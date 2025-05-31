#include "Transaction.h"
#include "Account.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockAccount : public Account { // gmoke нужно только для класса Account
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
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

TEST(Transaction, TransactionCallsAccountMethodsCorrectly){
    Transaction t;
    MockAccount acc1(0, 2201);
    MockAccount acc2(1, 1800);
    
    EXPECT_CALL(acc1, Lock()).Times(1);
    EXPECT_CALL(acc2, Lock()).Times(1);
    EXPECT_CALL(acc1, GetBalance())
        .WillOnce(::testing::Return(2201)
        .WillOnce(::testing::Return(2000));
    EXPECT_CALL(acc1, ChangeBalance(-201)).Times(1);
    EXPECT_CALL(acc2, ChangeBalance(200)).Times(1);
    EXPECT_CALL(acc1, Unlock()).Times(1);
    EXPECT_CALL(acc2, Unlock()).Times(1);
    EXPECT_CALL(acc2, GetBalance()).WillOnce(::testing::Return(2000));

    EXPECT_TRUE(t.Make(acc1, acc2, 200));
}

TEST(Transaction, NotEnoughFunds){
    Transaction t;
    MockAccount acc1(0, 1000);
    MockAccount acc2(1, 200);
    
    EXPECT_CALL(acc1, Lock()).Times(1);
    EXPECT_CALL(acc2, Lock()).Times(1);
    EXPECT_CALL(acc2, ChangeBalance(2000)).Times(1);
    EXPECT_CALL(acc1, GetBalance()).WillRepeatedly(::testing::Return(1000));
    EXPECT_CALL(acc2, GetBalance()).WillOnce(::testing::Return(200));
    EXPECT_CALL(acc1, ChangeBalance(-2001)).Times(0); // не должно быть вызова ChangeBalance, недостаточно средств
    EXPECT_CALL(acc2, ChangeBalance(-2000)).Times(1);
    EXPECT_CALL(acc1, Unlock()).Times(1);
    EXPECT_CALL(acc2, Unlock()).Times(1);
    
    
    EXPECT_FALSE(t.Make(acc1, acc2, 2000));
}

TEST(Transaction, IncorrectTransaction){
    Transaction t;
    MockAccount acc1(0, 1000);
    MockAccount acc2(1, 200);
    
    EXPECT_THROW(t.Make(acc1, acc1, 150), std::logic_error);
    EXPECT_THROW(t.Make(acc1, acc2, -150), std::invalid_argument);
    EXPECT_THROW(t.Make(acc1, acc2, 50), std::logic_error);
    
    t.set_fee(60);
    
    EXPECT_FALSE(t.Make(acc1, acc2, 115));
}


