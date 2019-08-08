#include "custom_handler.hpp"

Status status;

// convert ten byte floating point to double
double convert(BYTE* dump)
{
    BYTE data[10] = { 0, };
    std::copy(dump, dump + 10, data);

    double cash = 0;
    // __asm fld tbyte ptr[data];
    // __asm fstp qword ptr[cash];

    return cash;
}

void InModifyDoubleHandler::Handle(PCONTEXT context)
{
    status.CallLatestProcess(Process::MODIFY_DOUBLE, context);
}

void BtnSaleClickHandler::Handle(PCONTEXT context)
{
    if (status.GetLatestProcess() == Process::MODIFY_DOUBLE)
    {
        BYTE* dump = (BYTE*)(context->Rbp + 8);
        int cash = static_cast<int>(convert(dump));

        if (m_flag == Flag::RESET)
        {
            status.remain = 0;
            status.exchange = 0;
            status.cash_receive = 0;
            status.card_receive = 0;
            m_flag = Flag::DISCOUNT;
        }
        else if (m_flag == Flag::DISCOUNT)
        {
            status.pre_discount = cash;
            m_flag = Flag::REMAIN;
        }
        else if (m_flag == Flag::REMAIN)
        {
            status.remain = cash;
            if (status.pre_discount == status.remain)
            {
                status.pre_discount = 0;
            }

            status.total = status.remain + status.pre_discount;
            status.SetLatestProcess(Process::INVALID);
            m_flag = Flag::INVALID;

            printf("[*] Sale : Total - %d / Discount - %d / Remain - %d\n", status.total, status.pre_discount, status.remain);
            return;
        }

        status.SetLatestProcess(Process::BTN_SALE_CLICK);
        return;
    }

    status.SetLatestProcess(Process::BTN_SALE_CLICK);
    m_flag = Flag::RESET;
}

void BtnCashClickHandler::Handle(PCONTEXT context)
{
    if (status.GetLatestProcess() == Process::MODIFY_DOUBLE)
    {
        if (status.cash_receive != 0)
        {
            status.cash_receive = 0;

            status.exchange = 0;
            status.remain = (status.total - status.pre_discount) - status.card_receive;
            
            status.SetLatestProcess(Process::BTN_CASH_CLICK);
            return;
        }

        BYTE* dump = (BYTE*)(context->Rbp + 8);
        int cash = static_cast<int>(convert(dump));

        if (status.num_receive != 1)
        {
            cash -= status.card_receive;
        }

        status.cash_receive = cash;
        if (cash > status.remain)
        {
            status.exchange = cash - status.remain;
            cash = status.remain;
        }
        status.remain -= cash;

        printf("[*] Cash : Receive - %d / Remain - %d / Exchange - %d\n",
            status.cash_receive, status.remain, status.exchange);

        status.SetLatestProcess(Process::INVALID);
        return;
    }

    ++status.num_receive;
    status.SetLatestProcess(Process::BTN_CASH_CLICK);
}

void BtnCreditCardClickHandler::Handle(PCONTEXT context) 
{
    if (status.GetLatestProcess() == Process::MODIFY_DOUBLE)
    {
        BYTE* dump = (BYTE*)(context->Rbp + 8);
        int cash = static_cast<int>(convert(dump));

        if (m_pass && m_stack > 0)
        {
            if (m_stack == 1)
            {
                m_temporal = cash;
            }

            m_stack -= 1;
            status.SetLatestProcess(Process::BTN_CREDIT_CARD_CLICK);
            return;
        }

        if (m_pass && m_stack == 0)
        {
            status.card_receive = m_temporal - status.cash_receive;
            status.remain = cash;
        }
        else
        {
            status.card_receive = cash;
            status.remain -= cash;
        }

        printf("[*] Card : Receive - %d / Remain - %d / Exchange - %d\n",
            status.card_receive, status.remain, status.exchange);

        status.SetLatestProcess(Process::INVALID);
        return;
    }

    if (status.remain != status.total - status.pre_discount) 
    {
        m_pass = true;
        m_stack = ++status.num_receive + m_base;
    }
    else
    {
        m_pass = false;

        m_base = 2;
        m_stack = 0;
    }
    status.SetLatestProcess(Process::BTN_CREDIT_CARD_CLICK);
}

void BtnAccountClickHandler::Handle(PCONTEXT context)
{
    if (status.remain == 0)
    {
        printf("[*] Account\n");

        for (auto&[cName, price] : status.menus)
        {
            printf("%s : %d\n", cName.c_str(), price);
        }
        printf("total : %d\n", status.total);
        printf("discount : %d\n", status.pre_discount);

        printf("cash - %d / card - %d / exchange - %d\n",
            status.cash_receive, status.card_receive, status.exchange);
    }
}

void BtnAccountCancelClickHandler::Handle(PCONTEXT context) 
{
    status.exchange = 0;
    status.remain = status.total - status.pre_discount;
    
    status.num_receive;
    status.cash_receive = 0;
    status.card_receive = 0;

    status.SetLatestProcess(Process::BTN_ACCOUNT_CANCEL_CLICK);
}

void MenuClickHandler::Handle(PCONTEXT context)
{
    const char* wMenuName = *(const char**)(context->Rbp - 0x14);

    BYTE* dump = (BYTE*)(context->Rsp);
    int total = static_cast<int>(convert(dump));

    int price = total - status.total;
    status.total = total;

    if (price > 0) {
        printf("[*] Menu %s : price - %d / total - %d\n", wMenuName, price, total);
        status.menus.emplace_back(std::make_tuple(wMenuName, price));
    }
}

void BtnOrderCancelClickHandler::Handle(PCONTEXT context)
{
    status.menus.clear();
}

void WritePrinterHook::Handle(PCONTEXT context)
{
    size_t size = context->Rcx;
    BYTE* pBuffer = *(BYTE**)(context->Rdx);

    printf("[*] Print Dump : \n");
    for (int i = 0; i < size; ++i) {
        printf("%02X ", pBuffer[i]);
        if (i % 16 == 15) {
            printf("\n");
        }
    }
    printf("\n");
    for (int i = 0; i < size; ++i) {
        printf("%c", pBuffer[i]);
    }
}

void WriteFileHook::Handle(PCONTEXT context)
{
    size_t size = context->Rsi;
    BYTE* pBuffer = (BYTE*)context->Rax;

    for (size_t i = 0; i < size; ++i) {
        printf("%c", pBuffer[i]);
    }
}

Status::Status() : total(0), pre_discount(0), remain(0),
    num_receive(0), cash_receive(0), card_receive(0), discount(0), exchange(0),
    m_latest(Process::INVALID), m_lastMethod(Process::INVALID)
{
    m_list.emplace(Process::BTN_SALE_CLICK, std::make_unique<BtnSaleClickHandler>());
    m_list.emplace(Process::BTN_CASH_CLICK, std::make_unique<BtnCashClickHandler>());
    m_list.emplace(Process::BTN_CREDIT_CARD_CLICK, std::make_unique<BtnCreditCardClickHandler>());
}

Process Status::GetLatestProcess() const
{
    return m_latest;
}

void Status::SetLatestProcess(Process process)
{
    m_latest = process;
}

void Status::CallLatestProcess(Process from, PCONTEXT context)
{
    Call(from, GetLatestProcess(), context);
}

void Status::Call(Process from, Process to, PCONTEXT context)
{
    if (auto iter = m_list.find(to); iter != m_list.end())
    {
        SetLatestProcess(from);
        iter->second->Handle(context);
    }
}