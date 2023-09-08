#include<bits/stdc++.h>
#include <windows.h>
#include<string.h>
#include<stdio.h>
using namespace std;

string padding(string data_string,int block_size)
{
    while(data_string.size()%block_size!=0)
    {
        data_string+='~';
    }
    return data_string;
}
vector<vector<int>> data_block_gen(string data_string,int block_size,int row_bits)
{
    vector<vector<int>> data_block;
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
            data_block.push_back(temp);
        }
    }
    return data_block;

}
void print_data_block(vector<vector<int>> temp)
{

    for(int i=0;i<temp.size();i++)
    {
        for(int j=0;j<temp[i].size();j++)
        {
            cout<<temp[i][j]<<"\t";
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
        return false;
    }
    else
    {
        return true;
    }
}
vector<vector<int>> add_check_bits(vector<vector<int>> data_block,int block_size,int check_bits_count)
{
    int total=block_size+check_bits_count;
    for(int i=0;i<data_block.size();i++)
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
    return data_block;
}
void print_check_bit_data_block(vector<vector<int>> temp)
{

    for(int i=0;i<temp.size();i++)
    {
        for(int j=0;j<temp[i].size();j++)
        {
            if(parity_pos_check(j+1))
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                cout<<temp[i][j]<<"\t";
            }
            else
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                cout<<temp[i][j]<<"\t";
            }
            
        }
        cout<<endl;
    }
}
vector<int> collumn_major_serialization(vector<vector<int>> data_block)
{
    vector<int> serialized;
    for(int i=0;i<data_block[0].size();i++)
    {
        for(int j=0;j<data_block.size();j++)
        {
            serialized.push_back(data_block[j][i]);
        }
    }
    return serialized;
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
    vector<vector<int>> data_block;
    data_block=data_block_gen(data_string,m,8);
    cout<<"The Data Block: "<<endl;
    print_data_block(data_block);
    cout<<"The Data Block(after adding check bit): "<<endl;
    data_block=add_check_bits(data_block,m,3);
    print_check_bit_data_block(data_block);
    vector<int> serialized_data =  collumn_major_serialization(data_block);
    cout<<"After Column-Major Serialization: "<<endl;
    for(int i=0;i<serialized_data.size();i++)
    {
        cout<<serialized_data[i]<<"\t";
    }
    cout<<endl;
    return 0;
}