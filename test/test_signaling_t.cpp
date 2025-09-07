#include <gtest/gtest.h>

#include <tuple>

#include <kts/signaling_t/signaling_t.hpp>

template<typename T>
class LoggingListener : public kts::SignalingT<T>::Listener
{
public:
    void Update( const typename kts::SignalingT<T>::Event &event ) override
    {
        events.emplace_back( event );
    }

    auto &getEvents()
    {
        return events;
    }

    const auto &getEvents() const
    {
        return events;
    }

private:
    std::deque<typename kts::SignalingT<T>::Event> events;
};

template<typename T>
class SignalingTTest : public ::testing::Test
{
protected:
    T val{};
};

using SignalingTestTypes = ::testing::Types<int, double, std::string, bool, char>;
TYPED_TEST_SUITE( SignalingTTest, SignalingTestTypes );

TYPED_TEST( SignalingTTest, AllOperationsOnce )
{
    using T = std::remove_cv_t<std::remove_reference_t<decltype( this->val )>>;

    kts::SignalingT<T> move_from_this1;
    kts::SignalingT<T> move_from_this2;

    kts::SignalingT<T> copy_assigned;
    kts::SignalingT<T> move_assigned;
    kts::SignalingT<T> value_assigned;

    kts::SignalingT<T> swap_this1;
    kts::SignalingT<T> swap_this2;

    kts::SignalingT<T> compare_this1;
    kts::SignalingT<T> compare_this2;

    auto listener = LoggingListener<T>{};
    kts::SignalingT<T> default_constructed;
    kts::SignalingT<T> copy_constructed{ default_constructed };
    kts::SignalingT<T> move_constructed{ std::move( move_from_this1 ) };
    kts::SignalingT<T> value_constructed{ T{} };

    copy_assigned = default_constructed;
    move_assigned = std::move( move_from_this2 );
    value_assigned = T{};

    swap( swap_this1, swap_this2 );

    {
        listener.Disconnect();
        kts::SignalingT<T> destroyed{};
        listener.Connect();
    }

    std::ignore = compare_this1 == compare_this2;

    const auto &events{ listener.getEvents() };

    ASSERT_TRUE( events.size() == 10 );
    EXPECT_TRUE( std::holds_alternative<typename kts::SignalingT<T>::EventDefaultConstructed>( events.at( 0 ) ) );
    EXPECT_TRUE( std::holds_alternative<typename kts::SignalingT<T>::EventCopyConstructed>( events.at( 1 ) ) );
    EXPECT_TRUE( std::holds_alternative<typename kts::SignalingT<T>::EventMoveConstructed>( events.at( 2 ) ) );
    EXPECT_TRUE( std::holds_alternative<typename kts::SignalingT<T>::EventValueConstructed>( events.at( 3 ) ) );
    EXPECT_TRUE( std::holds_alternative<typename kts::SignalingT<T>::EventCopyAssigned>( events.at( 4 ) ) );
    EXPECT_TRUE( std::holds_alternative<typename kts::SignalingT<T>::EventMoveAssigned>( events.at( 5 ) ) );
    EXPECT_TRUE( std::holds_alternative<typename kts::SignalingT<T>::EventValueAssigned>( events.at( 6 ) ) );
    EXPECT_TRUE( std::holds_alternative<typename kts::SignalingT<T>::EventSwapped>( events.at( 7 ) ) );
    EXPECT_TRUE( std::holds_alternative<typename kts::SignalingT<T>::EventDestroyed>( events.at( 8 ) ) );
    EXPECT_TRUE( std::holds_alternative<typename kts::SignalingT<T>::EventCompared>( events.at( 9 ) ) );
}
