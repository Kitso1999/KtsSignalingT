#pragma once

#include <cstdint>
#include <deque>
#include <unordered_set>
#include <utility>
#include <variant>

namespace kts
{

template<typename T>
class SignalingT
{
public:
    using IdT = std::uint_fast64_t;

    struct EventDefaultConstructed
    {
        IdT id;
    };

    struct EventCopyConstructed
    {
        IdT id;
        IdT from_id;
    };

    struct EventMoveConstructed
    {
        IdT id;
        IdT from_id;
    };

    struct EventValueConstructed
    {
        IdT id;
        T from_value;
    };

    struct EventCopyAssigned
    {
        IdT id;
        IdT from_id;
    };

    struct EventMoveAssigned
    {
        IdT id;
        IdT from_id;
    };

    struct EventValueAssigned
    {
        IdT id;
        T from_value;
    };

    struct EventSwapped
    {
        IdT id;
        IdT with_id;
    };

    struct EventDestroyed
    {
        IdT id;
    };

    struct EventCompared
    {
        IdT id;
        IdT id_with;
    };

    using Event = std::variant<EventDefaultConstructed, EventCopyConstructed, EventMoveConstructed,
                               EventValueConstructed, EventCopyAssigned, EventMoveAssigned, EventValueAssigned,
                               EventSwapped, EventDestroyed, EventCompared>;

private:
    struct EventListenerConstructorKey
    {
    };

public:
    class EventListener
    {
    public:
        EventListener( [[maybe_unused]] EventListenerConstructorKey )
        {
            Connect();
        }

        EventListener( const EventListener & ) = delete;
        EventListener( EventListener && ) = delete;
        EventListener &operator=( const EventListener & ) = delete;
        EventListener &operator=( EventListener && ) = delete;

        ~EventListener()
        {
            Disconnect();
        }

        void Connect()
        {
            s_EventListeners.emplace( this );
        }

        void Disconnect()
        {
            s_EventListeners.erase( this );
        }

        auto &getEvents() noexcept
        {
            return events;
        }

        const auto &getEvents() const noexcept
        {
            return events;
        }

    private:
        std::deque<Event> events;
    };

    SignalingT() noexcept( std::is_nothrow_default_constructible_v<T> )
    {
        Emit( EventDefaultConstructed{ m_Id } );
    }

    SignalingT( const SignalingT &other ) noexcept( std::is_nothrow_copy_constructible_v<T> )
        : m_Value{ other.m_Value }
    {
        Emit( EventCopyConstructed{ m_Id, other.m_Id } );
    }

    SignalingT( SignalingT &&other ) noexcept( std::is_nothrow_move_constructible_v<T> )
        : m_Value{ std::move( other.m_Value ) }
    {
        Emit( EventMoveConstructed{ m_Id, other.m_Id } );
    }

    ~SignalingT() noexcept( std::is_nothrow_destructible_v<T> )
    {
        Emit( EventDestroyed{ m_Id } );
    }

    explicit SignalingT( T value )
        noexcept( std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_move_constructible_v<T> &&
                  std::is_nothrow_copy_assignable_v<T> && std::is_nothrow_move_assignable_v<T> )
        : m_Value{ value }
    {
        Emit( EventValueConstructed{ m_Id, std::move( value ) } );
    }

    SignalingT &operator=( const SignalingT &other ) noexcept( std::is_nothrow_copy_assignable_v<T> )
    {
        m_Value = other.m_Value;
        Emit( EventCopyAssigned{ m_Id, other.m_Id } );
        return *this;
    }

    SignalingT &operator=( SignalingT &&other ) noexcept( std::is_nothrow_move_assignable_v<T> )
    {
        m_Value = std::move( other.m_Value );
        Emit( EventMoveAssigned{ m_Id, other.m_Id } );
        return *this;
    }

    SignalingT &operator=( T value )
        noexcept( std::is_nothrow_copy_assignable_v<T> && std::is_nothrow_move_assignable_v<T> )
    {
        m_Value = value;
        Emit( EventValueAssigned{ m_Id, std::move( value ) } );
        return *this;
    }

    static EventListener Listen()
    {
        return { EventListenerConstructorKey{} };
    };

    const IdT &getId() const noexcept
    {
        return m_Id;
    }

    const T &getValue() const noexcept
    {
        return m_Value;
    }

    friend void swap( SignalingT &lhs, SignalingT &rhs ) noexcept( std::is_nothrow_swappable_v<T> )
    {
        using std::swap;
        swap( lhs.m_Value, rhs.m_Value );
        Emit( EventSwapped{ lhs.m_Id, rhs.m_Id } );
    }

    friend bool operator==( const SignalingT &lhs, const SignalingT &rhs )
        noexcept( noexcept( std::declval<T>() == std::declval<T>() ) )
    {
        Emit( EventCompared{ lhs.m_Id, rhs.m_Id } );
        return lhs.m_Value == rhs.m_Value;
    }

    friend bool operator!=( const SignalingT &lhs, const SignalingT &rhs )
        noexcept( noexcept( std::declval<T>() != std::declval<T>() ) )
    {
        Emit( EventCompared{ lhs.m_Id, rhs.m_Id } );
        return lhs.m_Value != rhs.m_Value;
    }

    friend bool operator<( const SignalingT &lhs, const SignalingT &rhs )
        noexcept( noexcept( std::declval<T>() < std::declval<T>() ) )
    {
        Emit( EventCompared{ lhs.m_Id, rhs.m_Id } );
        return lhs.m_Value < rhs.m_Value;
    }

    friend bool operator<=( const SignalingT &lhs, const SignalingT &rhs )
        noexcept( noexcept( std::declval<T>() <= std::declval<T>() ) )
    {
        Emit( EventCompared{ lhs.m_Id, rhs.m_Id } );
        return lhs.m_Value <= rhs.m_Value;
    }

    friend bool operator>( const SignalingT &lhs, const SignalingT &rhs )
        noexcept( noexcept( std::declval<T>() > std::declval<T>() ) )
    {
        Emit( EventCompared{ lhs.m_Id, rhs.m_Id } );
        return lhs.m_Value > rhs.m_Value;
    }

    friend bool operator>=( const SignalingT &lhs, const SignalingT &rhs )
        noexcept( noexcept( std::declval<T>() >= std::declval<T>() ) )
    {
        Emit( EventCompared{ lhs.m_Id, rhs.m_Id } );
        return lhs.m_Value >= rhs.m_Value;
    }

private:
    static void Emit( Event event )
    {
        for ( auto &listener : s_EventListeners )
            listener->getEvents().push_back( event );
    }

    inline static IdT s_IdCounter{};
    inline static std::unordered_set<EventListener *> s_EventListeners{};

    IdT m_Id{ s_IdCounter++ };
    T m_Value{};
};

} // namespace kts
