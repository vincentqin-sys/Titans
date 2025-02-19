#include "quote_demo.h"

#include <chrono>
#include <iostream>
#include <thread>

namespace demo {

QuoteDemo::QuoteDemo() : quote_api_(nullptr) { Init(); }

QuoteDemo::~QuoteDemo() {
    if (quote_api_) {
        quote_api_->Logout();
        quote_api_ = nullptr;
    }
}

void QuoteDemo::Init() {
    // 初始化行情类Api
    quote_api_ = EMQ::API::QuoteApi::CreateQuoteApi("./logs", EMQ_LOG_LEVEL::EMQ_LOG_LEVEL_DEBUG, EMQ_LOG_LEVEL::EMQ_LOG_LEVEL_DEBUG);
    if (!quote_api_) {
        std::cout << "CreatQuoteApi Failed!" << std::endl;
        return;
    }

    // 设置接收回报类SPI
    quote_api_->RegisterSpi(this);

    // 行情登录，请将您的行情账号密码填入
    auto error_code = quote_api_->Login("61.152.230.216", 8093, "user", "password");
    if (error_code < 0) {
        std::cout << "Quote Login Failed! error code: " << error_code << std::endl << std::endl;
        return;
    }
    std::cout << "Quote Login Success!" << std::endl;
}

void QuoteDemo::Run() {
    char *securityCodeSz[] = {(char *)"000001", (char *)"300059"};
    // 订阅指定标的快照行情
    quote_api_->SubscribeMarketData(securityCodeSz, 2, EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SZ);

    // 订阅深圳全市场指数快照行情
    quote_api_->SubscribeAllIndexData(EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SZ);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// 订阅快照行情响应
void QuoteDemo::OnSubMarketData(EMTSpecificTickerStruct *ticker, EMTRspInfoStruct *error_info, bool is_last) {
    std::cout << "----------OnSubMarketData----------" << std::endl;
    std::cout << "securityCode: " << ticker->ticker << std::endl
              << "error_id: " << error_info->error_id << std::endl
              << "error_msg: " << error_info->error_msg << std::endl
              << "is_last: " << is_last << std::endl; // 如果订阅多个标的，通过is_last判断订阅响应是否结束
}

// 订阅全市场指数快照行情响应
void QuoteDemo::OnSubscribeAllIndexData(EMQ_EXCHANGE_TYPE exchange_id, EMTRspInfoStruct *error_info) {
    std::cout << "--------------OnSubscribeAllIndexData------------------" << std::endl;
    std::cout << "exchangeType:" << static_cast<int32_t>(exchange_id) << std::endl
              << "error_id: " << error_info->error_id << std::endl
              << "error_msg: " << error_info->error_msg << std::endl;
}

// 现货集中竞价快照行情
void QuoteDemo::OnDepthMarketData(EMTMarketDataStruct *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[],
                                  int32_t ask1_count, int32_t max_ask1_count) {
    std::cout << "----------OnDepthMarketData----------" << std::endl
              << "exchage_type:" << static_cast<int>(market_data->exchange_id) << std::endl
              << "ticker:" << market_data->ticker << std::endl
              << "avg_price:" << market_data->avg_price << std::endl
              << "last_price:" << market_data->last_price << std::endl
              << "pre_close_price:" << market_data->pre_close_price << std::endl
              << "open_price:" << market_data->open_price << std::endl
              << "high_price:" << market_data->high_price << std::endl
              << "low_price:" << market_data->low_price << std::endl
              << "close_price:" << market_data->close_price << std::endl
              << "upper_limit_price:" << market_data->upper_limit_price << std::endl
              << "lower_limit_price:" << market_data->lower_limit_price << std::endl;

    if (market_data->exchange_id == EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SHHK || market_data->exchange_id == EMQ_EXCHANGE_TYPE::EMQ_EXCHANGE_SZHK) {
        std::cout << "refer_price: " << market_data->hk.refer_price << std::endl
                  << "nominal_Price:" << market_data->hk.nominal_price << std::endl
                  << "buySideUpLimitPrice:" << market_data->hk.buyside_up_limit_price << std::endl
                  << "buySideLowLimitPrice:" << market_data->hk.buyside_low_limit_price << std::endl
                  << "sellSideUpLimitPrice:" << market_data->hk.sellside_up_limit_price << std::endl
                  << "sellSideLowLimitPrice:" << market_data->hk.sellside_low_limit_price << std::endl;
    }
}

// 指数快照行情
void QuoteDemo::OnIndexData(EMTIndexDataStruct *index_data) {
    std::cout << "----------OnIndexData-----------------" << std::endl;
    std::cout << index_data->ticker << std::endl
              << index_data->last_price << std::endl;
}

} // namespace demo