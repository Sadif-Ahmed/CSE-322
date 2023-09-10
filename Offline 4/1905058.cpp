#include<bits/stdc++.h>
#include <windows.h>
#include<string.h>
#include<stdio.h>
#include<random>
using namespace std;

string padding(string data_string,int block_size)
{
    while(data_string.size()%block_size!=0)
    {
        data_string+='~';
    }
    return data_string;
}
void data_block_gen(vector<int>*data_block,string data_string,int block_size,int row_bits)
{
    
    int row_count=-1;
    for(int i=0;i<data_string.size();i++)
    {
        if(i%block_size==0)
        {
            row_count++;
        }
        int bit_mask=1;
        vector<int> temp;
        char temp_c=data_string[i];
        for(int j=row_bits-1;j>=0;j--)
        {
            bit_mask=bit_mask<<j;
            if((temp_c&bit_mask) == 0)
            {
                temp.push_back(0);
            }
            else
            {
                temp.push_back(1);
            }
            bit_mask=1;
        }
        for(int j=0;j<row_bits;j++)
        {
            data_block[row_count].push_back(temp[j]);
        }
    }

}
void print_data_block(vector<int> *temp,int row_count)
{

    for(int i=0;i<row_count;i++)
    {
        for(int j=0;j<temp[i].size();j++)
        {
            cout<<temp[i][j];
        }
        cout<<endl;
    }
}
bool parity_pos_check(int pos)
{
    while(pos%2==0)
    {
        pos=pos/2;
    }
    if(pos==1)
    {
        return true;
    }
    else
    {
        return false;
    }
}
int add_check_bits(vector<int>* data_block,int block_size,int check_bits_count,int row_count)
{
    int total=block_size+check_bits_count;
    for(int i=0;i<row_count;i++)
    {
        for(int j=0;j<total;j++)
        {
            int bit_mask=1;
            bit_mask=bit_mask<<j;
            bit_mask--;
            if(bit_mask>data_block[i].size())
            {
                total--;
                continue;
            }
            data_block[i].insert(data_block[i].begin()+bit_mask,0);
        }
        for(int k=0; k<total; k++)
        {
            int bit_mask=1;
            bit_mask=bit_mask<<k;
            int parity_bit=0;
            
            for(int j=0; j<data_block[i].size(); j++)
            {
                if(((j+1)&bit_mask)!=0)
                {
                    
                    parity_bit^=data_block[i][j];
                }

            }
            bit_mask--;
            data_block[i][bit_mask]=parity_bit;
        }
    }
    return total;
    
}
void print_check_bit_data_block(vector<int>* temp,int row_count)
{

    for(int i=0;i<row_count;i++)
    {
        for(int j=0;j<temp[i].size();j++)
        {
            if(parity_pos_check(j+1))
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                cout<<temp[i][j];
            }
            else
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                cout<<temp[i][j];
            }
            
        }
        cout<<endl;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
}
vector<int> collumn_major_serialization(vector<int>* data_block,int row_count)
{
    vector<int> serialized;
    for(int i=0;i<data_block[0].size();i++)
    {
        for(int j=0;j<row_count;j++)
        {
            serialized.push_back(data_block[j][i]);
        }
    }
    return serialized;
}
void print_serialized(vector<int> serialized_data)
{
    for(int i=0;i<serialized_data.size();i++)
    {
        cout<<serialized_data[i];
    }
}
vector<int> crc_checksum_gen(vector<int> serialized_data,string generator)
{
    
    vector<int> appended_data=serialized_data;
    for(int i=0;i<generator.size()-1;i++)
    {
        appended_data.push_back(0);
    }
    for(int i=0;i<=appended_data.size()-generator.size();i++)
    {
        if(appended_data[i]==0)
        {
            continue;
        }
        for(int j=1;j<i+generator.size();j++)
        {
            appended_data[j]^=((char)generator[j-i]-'0');
        }
    }
    vector<int> R;
    for(int i=serialized_data.size();i<appended_data.size();i++)
    {
            R.push_back(appended_data[i]);
    }
    appended_data.clear();
    appended_data=serialized_data;
    for(int i=0;i<R.size();i++)
    {
        appended_data.push_back(R[i]);
    }
    return appended_data;
}
void print_crc_checksum(vector<int> serialized,vector<int> appended)
{
    for(int i=0;i<appended.size();i++)
    {
        if(i>=serialized.size())
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
            cout<<appended[i];
        }
        else
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
            cout<<appended[i];
        }
        
    }
     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
}
double rand_gen(double p)
{
    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator
    uniform_real_distribution<> distr(0, 1); // define the range
    return distr(gen);
}
pair<vector<int>,vector<int>> transmission(vector<int> frame,double error_p)
{
    vector<int> received_frame=frame;
    vector<int>toggle_pos;
    for(int i=0;i<frame.size();i++)
    {
        if(rand_gen(error_p)<error_p)
        {
            toggle_pos.push_back(i);
            if(frame[i]==0)
            {
                received_frame[i]=1;
            }
            else
            {
                received_frame[i]=0;
            }
        }
    }
    return make_pair(received_frame,toggle_pos);
}
void print_received_frame(vector<int>received,vector<int>toggle_pos)
{
    bool flag;
    for(int i=0;i<received.size();i++)
    {
        flag=false;
        for(int j=0;j<toggle_pos.size();j++)
        {
            if(i==toggle_pos[j])
            {
                flag=true;
                break;
            }
        }
        if(flag)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            cout<<received[i];
        }
        else
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
            cout<<received[i];   
        }
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
}
bool crc_checksum_check(vector<int> frame,string generator)
{
    vector<int> appended=frame;
    for(int i=0;i<=appended.size()-generator.size();i++)
    {
        if(appended[i]==0)
        {
            continue;
        }
        for(int j=1;j<i+generator.size();j++)
        {
            appended[j]^=((char)generator[j-i]-'0');
        }
    }
    for(int i=0;i<appended.size();i++)
    {
        if(appended[i])
        {
            return false;
        }
    }
    return true;
}
vector<int> crc_checksum_rem(vector<int> frame,int frame_size)
{
    vector<int> ret_frame;
    for(int i=0;i<frame_size;i++)
    {
        ret_frame.push_back(frame[i]);
    }
    return ret_frame;
}
void deserialize(vector<int>*data_block,int row_count,int col_count,vector<int> serialized)
{
    for(int i=0;i<row_count;i++)
    {
        for(int j=0;j<col_count;j++)
        {
            data_block[i].push_back(serialized[i+row_count*j]);
        }
    }
}
void print_desiarlized_block(vector<int>*data_block,int row_count,vector<int> toggle_pos)
{
    bool flag;
    for(int i=0;i<row_count;i++)
    {
        for(int j=0;j<data_block[i].size();j++)
        {
            flag=false;
            for(int k=0;k<toggle_pos.size();k++)
        {
            if((i+j*row_count)==toggle_pos[k])
            {
                flag=true;
                break;
            }
        }
        if(flag)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            cout<<data_block[i][j];
        }
        else
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
            cout<<data_block[i][j];   
        }   
        }
        cout<<endl;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
}
vector<int> error_correction(vector<int>* data_block,int row_count,int num_check_bits)
{
    
    vector<int> parity_pos;
    for(int i=0;i<row_count;i++)
    {
    
    vector<int> parity_mismatch;
    parity_pos.clear();
       
        for(int k=0; k<num_check_bits; k++)
        {
            int bit_mask=1;
            bit_mask=bit_mask<<k;
            int parity_bit=0;
            
            for(int j=0; j<data_block[i].size(); j++)
            {
                if(((j+1)&bit_mask)!=0)
                {
                    
                    parity_bit^=data_block[i][j];
                }

            }
            bit_mask--;  
            parity_mismatch.push_back(parity_bit);
            parity_pos.push_back(bit_mask);
    
        }
        int error_pos=0;
        int temp=1;
        for(int i=0;i<parity_mismatch.size();i++)
        {
            error_pos+=(temp*parity_mismatch[i]);
            temp*=2;
        }
        error_pos--;
        //cout<<"At Row"<<i+1<<" Error at pos: "<<error_pos<<endl;
        if(error_pos<0 ||  error_pos>=(data_block[0].size()-num_check_bits))
        {
            continue;
        }
        if(data_block[i][error_pos]==1)
        {
            data_block[i][error_pos]=0;
        }
        else
        {
            data_block[i][error_pos]=1;
        }
        
    }
    return parity_pos;
    
}
void remove_check_bits(vector<int> *old_table,vector<int>* new_table,int row_count)
{
     
    for(int i=0;i<row_count;i++)
    {
        for(int j=0;j<old_table[i].size();j++)
        {
           if(!parity_pos_check(j+1))
            {  
               new_table[i].push_back(old_table[i][j]);
            }
           
        }
    }
}
string data_block_to_string(vector<int>* data_table,int row_count,int bits_per_char)
{
    string ret="";
    for(int i=0;i<row_count;i++)
    {
        bitset<8> ch;
        for(int j=0;j<data_table[i].size();j+=bits_per_char)
        {
            for(int k=j;k<j+bits_per_char;k++)
            {
                ch[bits_per_char-1-k+j]=data_table[i][k];
            }
            ret.push_back((char)ch.to_ulong());
        }
    }
    return ret;
}
int main()
{
    string data_string;
    int m;
    double p;
    string gen_poly;

    cout<<"Enter input values in order(data string,m,p,geneartor_polynomial):"<<endl;

    getline(cin,data_string);
    cin>>m;
    cin>>p;
    cin>>gen_poly;
    
    cout<<"The Data String:  "<<data_string<<endl;
    cout<<"The data block row size:  "<<m<<endl;
    cout<<"The probability of bit flipping: "<<p<<endl;
    cout<<"The generator polynomial:  "<<gen_poly<<endl;
    if(data_string.size()!=m)
    {
        cout<<"Data string block size mismatch."<<endl;
        data_string=padding(data_string,m);
        cout<<"The Data String(after Padding): "<<data_string<<endl;
    }
    vector<int> data_block[data_string.size()/m];
    data_block_gen(data_block,data_string,m,8);
    cout<<"The Data Block: "<<endl;
    print_data_block(data_block,data_string.size()/m);
    cout<<"The Data Block(after adding check bit): "<<endl;
    int num_check_bits=add_check_bits(data_block,m,3,data_string.size()/m);
    cout<<"Number of Check bits added per row: "<<num_check_bits<<endl;
    print_check_bit_data_block(data_block,data_string.size()/m);
    vector<int> serialized_data =  collumn_major_serialization(data_block,data_string.size()/m);
    cout<<"After Column-Major Serialization: "<<endl;
    print_serialized(serialized_data);
    cout<<endl;
    cout<<"After CRC Checksum Addition:"<<endl;
    vector<int> crc = crc_checksum_gen(serialized_data,gen_poly);
    print_crc_checksum(serialized_data,crc);
    cout<<endl;
    cout<<"At the receiving End: "<<endl;
    pair<vector<int>,vector<int>> trans = transmission(crc,p);
    print_received_frame(trans.first,trans.second);
    cout<<endl;
    cout<<"Result of Checksum Check: ";
    if(crc_checksum_check(trans.first,gen_poly))
    {
        cout<<"No Error Detected"<<endl;
    }
    else
    {
        cout<<"Error Detected"<<endl;
    }
    vector<int> checksum_removed = crc_checksum_rem(trans.first,serialized_data.size());
    vector<int> err_data_block[data_string.size()/m];
    vector<int> re_data_block[data_string.size()/m];
    deserialize(err_data_block,data_string.size()/m,data_block[0].size(),checksum_removed);
    cout<<"After Removing Checksum and Deserialization: "<<endl;
    print_desiarlized_block(err_data_block,data_string.size()/m,trans.second);

    
    cout<<"After Removing Check Bits:"<<endl;
    vector<int> parity_pos=error_correction(err_data_block,data_string.size()/m,num_check_bits);
    remove_check_bits(err_data_block,re_data_block,data_string.size()/m);
    print_data_block(re_data_block,data_string.size()/m);
    
    cout<<"Output Frame: "<<endl;
    string message=data_block_to_string(re_data_block,data_string.size()/m,8);
    cout<<message<<endl;
    return 0;
}