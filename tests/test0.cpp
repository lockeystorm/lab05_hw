#include "Transaction.h"
#include "Account.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockAccount : public Account { // gmoke нужно только для класса Account
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const));
    MOCK_METHOD(void, ChangeBalance, (int));
    MOCK_METHOD(void, Lock, (), ());
    MOCK_METHOD(void, Unlock, (), ());
};

TEST(Account, ChangeBalanceWithoutLock) {
    Account acc(1, 100);
    EXPECT_THROW(acc.ChangeBalance(50), std::runtime_error);
    
    acc.Lock();
    EXPECT_NO_THROW(acc.ChangeBalance(50));
    EXPECT_EQ(acc.GetBalance(), 150);
}

TEST(Account, DoubleLock) {
    Account acc(1, 100);
    acc.Lock();
    EXPECT_THROW(acc.Lock(), std::runtime_error);
    
    acc.Unlock();
    EXPECT_NO_THROW(acc.Lock());
}

TEST(Account, ChangeBalanceCorrectly) {
    Account acc(0, 100);
    EXPECT_EQ(acc.GetBalance(), 100);

    acc.Lock();
    EXPECT_NO_THROW(acc.ChangeBalance(50));
    EXPECT_EQ(acc.GetBalance(), 150);
}

TEST(Transaction, SetFee){
    Transaction t;
    EXPECT_EQ(t.fee(), 1);

    t.set_fee(12);
    EXPECT_EQ(t.fee(), 12);
}

TEST(Transaction, IncorrectTransaction){
    Transaction t;
    Account acc1(0, 1000);
    Account acc2(1, 200);
    
    EXPECT_THROW(t.Make(acc1, acc1, 150), std::logic_error);
    EXPECT_THROW(t.Make(acc1, acc2, -150), std::invalid_argument);
    EXPECT_THROW(t.Make(acc1, acc2, 50), std::logic_error);
    
    t.set_fee(60);
    
    EXPECT_FALSE(t.Make(acc1, acc2, 115));
    EXPECT_FALSE(t.Make(acc1, acc2, 2000));
}

TEST(Transaction, CorrectTransaction){
    Transaction t;
    Account acc1(0, 2201);
    Account acc2(1, 1800);
    
    EXPECT_TRUE(t.Make(acc1, acc2, 200));
    EXPECT_EQ(acc1.GetBalance(), 2000);
    EXPECT_EQ(acc2.GetBalance(), 2000);
}

