#ifndef CUSTOM_HANDLER_HPP
#define CUSTOM_HANDLER_HPP

#include <memory>
#include <unordered_map>

#include <handler.hpp>

enum class Process {
    INVALID,
    MODIFY_DOUBLE,
    BTN_SALE_CLICK,
    BTN_CASH_CLICK,
    BTN_CREDIT_CARD_CLICK,
    BTN_ACCOUNT_CANCEL_CLICK,
};

struct InModifyDoubleHandler : Handler {
    void Handle(PCONTEXT context) override;
};

struct BtnSaleClickHandler : Handler {
    void Handle(PCONTEXT context) override;

    enum class Flag {
        INVALID,
        RESET,
        DISCOUNT,
        REMAIN,
    } m_flag;
};

struct BtnCashClickHandler : Handler {
    void Handle(PCONTEXT context) override;
};

struct BtnCreditCardClickHandler : Handler {
    void Handle(PCONTEXT context) override;

    int m_temporal;

    int m_base = 1;
    int m_stack;
    bool m_pass;
};

struct BtnAccountClickHandler : Handler {
    void Handle(PCONTEXT context) override;
};

struct BtnAccountCancelClickHandler : Handler {
    void Handle(PCONTEXT conext) override;
};

struct MenuClickHandler : Handler {
    void Handle(PCONTEXT context) override;
};

struct BtnOrderCancelClickHandler : Handler {
    void Handle(PCONTEXT context) override;
};

struct WritePrinterHook : Handler {
    void Handle(PCONTEXT context) override;
};

struct WriteFileHook : Handler {
    void Handle(PCONTEXT context) override;
};

class Status {
public:
    int total = 0;
    int pre_discount = 0;
    int remain = 0;

    int num_receive = 0;
    int cash_receive = 0;
    int card_receive = 0;

    int discount = 0;
    int exchange = 0;

    std::vector<std::tuple<std::string, int>> menus;

    Process GetLatestProcess() const;
    void SetLatestProcess(Process process);
    void CallLatestProcess(Process from, PCONTEXT context);

    void Call(Process from, Process to, PCONTEXT context);

private:
    Process m_latest = Process::INVALID;
    Process m_lastMethod = Process::INVALID;

    std::unordered_map<Process, std::unique_ptr<Handler>> m_list = {
        { Process::BTN_SALE_CLICK, std::make_unique<BtnSaleClickHandler>() },
        { Process::BTN_CASH_CLICK, std::make_unique<BtnCashClickHandler>() },
        { Process::BTN_CREDIT_CARD_CLICK, std::make_unique<BtnCreditCardClickHandler>() },
    };
};

#endif