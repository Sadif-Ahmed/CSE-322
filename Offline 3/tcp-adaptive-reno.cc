#include "tcp-adaptive-reno.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "rtt-estimator.h"
#include "tcp-socket-base.h"

NS_LOG_COMPONENT_DEFINE ("TcpAdaptiveReno");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TcpAdaptiveReno);

TypeId
TcpAdaptiveReno::GetTypeId (void)
{
  static TypeId tid = TypeId("ns3::TcpAdaptiveReno")
    .SetParent<TcpNewReno>()
    .SetGroupName ("Internet")
    .AddConstructor<TcpAdaptiveReno>()
    .AddAttribute("FilterType", "Use this to choose no filter or Tustin's approximation filter",
                  EnumValue(TcpAdaptiveReno::TUSTIN), MakeEnumAccessor(&TcpAdaptiveReno::m_fType),
                  MakeEnumChecker(TcpAdaptiveReno::NONE, "None", TcpAdaptiveReno::TUSTIN, "Tustin"))
    .AddTraceSource("EstimatedBW", "The estimated bandwidth",
                    MakeTraceSourceAccessor(&TcpAdaptiveReno::m_currentBW),
                    "ns3::TracedValueCallback::Double")
  ;
  return tid;
}

TcpAdaptiveReno::TcpAdaptiveReno (void) :
  TcpWestwoodPlus(),
  m_min_round_trip_time (Time (0)),
  m_current_round_trip_time (Time (0)),
  m_j_packet_loss_round_trip_time (Time (0)),
  m_congestion_round_trip_time (Time (0)),
  m_prev_congestion_round_trip_time (Time(0)),
  m_inc_window (0),
  m_base_window (0),
  m_probe_window (0)
{
  
}

TcpAdaptiveReno::TcpAdaptiveReno (const TcpAdaptiveReno& sock) :
  TcpWestwoodPlus (sock),
  m_min_round_trip_time (Time (0)),
  m_current_round_trip_time (Time (0)),
  m_j_packet_loss_round_trip_time (Time (0)),
  m_congestion_round_trip_time (Time (0)),
  m_prev_congestion_round_trip_time (Time(0)),
  m_inc_window (0),
  m_base_window (0),
  m_probe_window (0)
{
}

TcpAdaptiveReno::~TcpAdaptiveReno (void)
{
}

/*
The function is called every time an ACK is received (only one time
also for cumulative ACKs) and contains timing information
It will increase the count of acked segments and update the current estimated bandwidth
*/
void
TcpAdaptiveReno::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t packetsAcked,
                        const Time& rtt)
{

  if (rtt.IsZero ())
    {
      NS_LOG_WARN ("RTT measured is zero!");
      return;
    }

  m_ackedSegments += packetsAcked;

  /*

      INITIALIZE AND SET VALUES FOR  m_minRtt, m_currentRtt

  */

  // calculate min rtt here
  if(m_min_round_trip_time.IsZero()) { m_min_round_trip_time = rtt; }
  else if(rtt <= m_min_round_trip_time) { m_min_round_trip_time = rtt; }

  m_current_round_trip_time = rtt;

  EstimateBW (rtt, tcb);
}


double
TcpAdaptiveReno::EstimateCongestionLevel()
{
  /*

      VARIABLE a = EXPONENTIAL SMOOTHING FACTOR
      m_congestion_RTT = (1-a)*m_curr_RTT + a*m_prev_congestion_RTT
      
      RETURN:
      congestion_level = min(((m_curr_Rtt - m_min_Rtt) / (m_congestion_RTT - m_min_Rtt)), 1)

  */

  float a = 0.85; // exponential smoothing factor
  if(m_prev_congestion_round_trip_time < m_min_round_trip_time) a = 0; // the initial value should take the full current Jth loss Rtt
  
  double congetion_RTT = a*m_prev_congestion_round_trip_time.GetSeconds() + (1-a)*m_j_packet_loss_round_trip_time.GetSeconds();
  m_congestion_round_trip_time = Seconds(congetion_RTT); // for next step calculation

  double congestion_level=(m_current_round_trip_time.GetSeconds() - m_min_round_trip_time.GetSeconds()) / (congetion_RTT - m_min_round_trip_time.GetSeconds());
  if(congestion_level>1.0)
  {
    congestion_level=1;
  }
  return congestion_level;
}


void 
TcpAdaptiveReno::EstimateIncWnd(Ptr<TcpSocketState> tcb)
{
  /*

      c = EstimateCongestionLevel() // congestion level
      scalingFactor_m = 10 // in Mbps
      m_maxIncWnd = EstimateBW() / scalingFactor_m

      alpha = 10 
      beta = 2 * m_maxIncWnd * (1/alpha - (1/alpha + 1)/(e^alpha))
      gamma = 1 - 2 * m_maxIncWnd * (1/alpha - (1/alpha + 1/2)/(e^alpha))

      m_incWnd = (m_maxIncWnd / (e^(alpha * c))) + (beta * c) + gamma

  */

  double congestion = EstimateCongestionLevel();
  int scalingFactor_m = 1000; // 10 mbps in paper 
  
  // m_currentBW; -> already calculated in packetsAck
double m_maxIncWnd = m_currentBW.Get().GetBitRate() / scalingFactor_m * (std::pow(tcb->m_segmentSize, 2)*8);

  alpha = 10; // 2 10
  beta = 2 * m_maxIncWnd * ((1/alpha) - ((1/alpha + 1)/(std::exp(alpha))));
  gamma = 1 - (2 * m_maxIncWnd * ((1/alpha) - ((1/alpha + 0.5)/(std::exp(alpha)))));

  m_inc_window = (int)((m_maxIncWnd / std::exp(alpha * congestion)) + (beta * congestion) + gamma);
}


void
TcpAdaptiveReno::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  /*
  
      base_window = USE NEW RENO IMPLEMENTATION
      m_probeWnd = max(m_probeWnd + m_incWnd / current_window,  0)
      current_window = base_window + m_probeWnd

  */

  if (segmentsAcked > 0)
    {
      EstimateIncWnd(tcb);
      // base_window = USE NEW RENO IMPLEMENTATION
      double adder = (std::pow(tcb->m_segmentSize, 2)*8) / tcb->m_cWnd.Get();
      if(adder<1.0)
      {
        adder=1.0;
      }
      m_base_window += static_cast<uint32_t> (adder);

      // change probe window
      m_probe_window = (double) (m_probe_window + m_inc_window / (int)tcb->m_cWnd.Get());
      if(m_probe_window<0)
      {
        m_probe_window=0;
      }
      tcb->m_cWnd = m_base_window + m_probe_window;
    }

}

uint32_t
TcpAdaptiveReno::GetSsThresh (Ptr<const TcpSocketState> tcb,
                          uint32_t bytesInFlight)
{
  /*
  
      c = EstimateCongestionLevel() // congestion level
      RETURN:
      new_window = current_window / (1 + c)

  */
  m_prev_congestion_round_trip_time = m_congestion_round_trip_time; // a loss event has occured. so set the previous conjestion RTT
  m_j_packet_loss_round_trip_time = m_current_round_trip_time; // this will now contain the RTT of previous packet or jth loss event
  
  double congestion = EstimateCongestionLevel();
  // NS_LOG_UNCOND("SSThresh called");

  uint32_t ssthresh = std::max (
    2*tcb->m_segmentSize*8,
    (uint32_t) (tcb->m_cWnd / (1.0+congestion))
  );

  // reset calculations
  m_base_window = ssthresh;
  m_probe_window = 0;
  
  return ssthresh;

}

Ptr<TcpCongestionOps>
TcpAdaptiveReno::Fork ()
{
  return CreateObject<TcpAdaptiveReno> (*this);
}

} // namespace ns3