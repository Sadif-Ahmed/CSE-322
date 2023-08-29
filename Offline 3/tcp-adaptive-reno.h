#ifndef TCP_ADAPTIVERENO_H
#define TCP_ADAPTIVERENO_H

#include "tcp-congestion-ops.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/sequence-number.h"
#include "ns3/traced-value.h"
#include "ns3/event-id.h"
#include "tcp-westwood-plus.h"

namespace ns3 {

class Packet;
class TcpHeader;
class Time;
class EventId;

/**
 * \ingroup congestionOps
 *
 * \brief An implementation of TCP ADAPTIVE RENO.
 *
 */
class TcpAdaptiveReno : public TcpWestwoodPlus
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  TcpAdaptiveReno (void);
  /**
   * \brief Copy constructor
   * \param sock the object to copy
   */
  TcpAdaptiveReno (const TcpAdaptiveReno& sock);
  virtual ~TcpAdaptiveReno (void);

  /**
   * \brief Filter type (None or Tustin)
   */
  enum FilterType 
  {
    NONE,
    TUSTIN
  };

  virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb,
                                uint32_t bytesInFlight);

  virtual void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t packetsAcked,
                          const Time& rtt);

  virtual Ptr<TcpCongestionOps> Fork ();


private:

  double EstimateCongestionLevel();

  void EstimateIncWnd(Ptr<TcpSocketState> tcb);

protected:
  virtual void CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
  //Round Trip Times
  Time                   m_min_round_trip_time;                 //!< Minimum Round Trip Time
  Time                   m_current_round_trip_time;             //!< Current Round Trip Time
  Time                   m_j_packet_loss_round_trip_time;            //!< Packet Loss Round Trip Time(j th loss)
  Time                   m_congestion_round_trip_time;                //!< Congestion Round Trip Time (j th event)
  Time                   m_prev_congestion_round_trip_time;            //!< Previous Congestion Rounf Trip Time (j-1 th event)
  //Equation Parametres
  double                 alpha;
  double                 beta;
  double                 gamma;
  // Window calculations
  int32_t                m_inc_window;                 //!< Increment Window
  uint32_t               m_base_window;                //!< Base Window
  int32_t                m_probe_window;               //!< Probe Window 
};

} // namespace ns3

#endif /* TCP_ADAPTIVE_RENO_H */