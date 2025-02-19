﻿#include <iostream>
#include <unistd.h>
#include <set>
#include <glog/logging.h>

#include "ti_quote_callback.h"
#include "ti_quote_depth_callback.h"
#include "ti_quote_ipc_client.h"
#include "ti_quote_formater.h"

class UserCallback : 
    public TiQuoteCallback, public TiQuoteDepthCallback
{
private:
    int64_t m_cout_future_time;
    int64_t m_cout_snap_time;
    int64_t m_cout_match_time;
    int64_t m_cout_order_time;
    int64_t m_cout_order_book_time;
    std::set<std::string> m_selected_symbols;
public:
    UserCallback(){
        m_cout_future_time = 0;
        m_cout_snap_time = 0;
        m_cout_match_time = 0;
        m_cout_order_time = 0;
        m_cout_order_book_time = 0;
        /*
        m_selected_symbols.insert("600000");
        m_selected_symbols.insert("000001");
        m_selected_symbols.insert("688981");
        m_selected_symbols.insert("300152");
        m_selected_symbols.insert("873679");
        m_selected_symbols.insert("400174");  
        //*/
        /*
        301129
        301181
        301261
        301317
        301393
        301397
        688075
        688136
        688137
        688162
        688355
        */

        /*
        m_selected_symbols.insert("301129");
        m_selected_symbols.insert("301181");
        m_selected_symbols.insert("301261");
        m_selected_symbols.insert("301317");
        m_selected_symbols.insert("301393");
        m_selected_symbols.insert("301397");
        m_selected_symbols.insert("688075");
        m_selected_symbols.insert("688136");
        m_selected_symbols.insert("688137");
        m_selected_symbols.insert("688162");
        m_selected_symbols.insert("688355");  

        m_selected_symbols.insert("000016");   
        m_selected_symbols.insert("000300");   
        m_selected_symbols.insert("000905");   
        m_selected_symbols.insert("000852");   

        m_selected_symbols.insert("399905");  
        */
        //m_selected_symbols.insert("399300");  
    };
    virtual ~UserCallback(){};
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){
        auto iter = m_selected_symbols.find(pData->symbol);
        if(iter == m_selected_symbols.end()){
            return;
        }
        printf("[OnL2IndexSnapshotRtn] %s, %s, %d, %s, %f, %f\n", 
            pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->pre_close);

    };

    
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
    {
        //return;
        if ((pData->time - m_cout_future_time) > 5000)
        {
            printf("[OnL2FutureSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
            m_cout_future_time = pData->time;
        }
    };

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
        /*
        auto iter = m_selected_symbols.find(pData->symbol);
        if(iter == m_selected_symbols.end()){
            return;
        }
        */
        /*
        if (strcmp(pData->exchange, "BJ"))
        {
            return;
        }
        */
        if ((pData->time - m_cout_snap_time) > 5000)
        {
            printf("[OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f, %f, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover,
                pData->high_limit, pData->low_limit);
            //json j;
            //TiQuoteFormater::FormatSnapshot(pData, j);
            //std::cout << j << std::endl;
            m_cout_snap_time = pData->time;
        }
    };
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
        //return;
        if ((pData->time - m_cout_match_time) > 5000)
        {     
            printf("[OnL2StockMatchesRtn] %s, %s, %s, %d, %d, %f, %d, %c, %c\n", 
                pData->symbol, pData->exchange, pData->time_str, pData->time, pData->seq, pData->price, pData->volume, 
                pData->bs_flag, pData->function_code);
            m_cout_match_time = pData->time;
        }
    };
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){
        //return;
        if ((pData->time - m_cout_order_time) > 5000)
        {
            printf("[OnL2StockOrderRtn] %s, %s, %s, %d, %d, %f, %d, %c, %c\n", 
                pData->symbol, pData->exchange, pData->time_str, pData->time,  pData->seq, pData->price, pData->volume, 
                pData->order_type, pData->function_code);
            m_cout_order_time = pData->time;
        }
    };

    
    virtual void OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
        const std::vector<TiQuoteDepthLevel*> &asks, 
        const std::vector<TiQuoteDepthLevel*> &bids){};
    
    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData){
        //return;
        if ((pData->time - m_cout_order_book_time) > 1000)
        {
            printf("[OnDepthOrderBookLevelRtn] %s, %s, %s, %d, %d, ask:[%f, %d, %d], bid:[%f, %d, %d], ask order {%d, %d, %d, %d}, bid order {%d, %d, %d, %d}\n", 
                pData->symbol, pData->exchange, pData->time_str, pData->time, pData->level,
                pData->ask_price, pData->ask_volume, pData->ask_order_count, pData->bid_price, pData->bid_volume, pData->bid_order_count,
                pData->ask_orders[0].seq, pData->ask_orders[0].order_orino, pData->ask_orders[0].volume, pData->ask_orders[0].dealt_vol,
                pData->bid_orders[0].seq, pData->bid_orders[0].order_orino, pData->bid_orders[0].volume, pData->bid_orders[0].dealt_vol
            );
            
            m_cout_order_book_time = pData->time;
        }
    };
};

int main(int argc, char* argv[]) {
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    UserCallback cb;

    TiQuoteIpcClient client("./config.ini", loop, &cb, &cb);
	
    if (argc == 2)
    {
        client.run(argv[1]);
    }else{
        client.run(NULL);
    }
    
    return uv_run(loop, UV_RUN_DEFAULT);
}
