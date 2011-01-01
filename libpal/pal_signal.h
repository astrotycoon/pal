#ifndef __PAL_SIGNAL_H__
#define __PAL_SIGNAL_H__

#include "libpal/palArray.h"
#include "libpal/pal_delegate.h"

template< class Param0 = void >
class palSignal0
{
public:
	typedef _pal_delegate_internal::palDelegate0< void > _Delegate;
private:
	palArray<_Delegate> _delegates;
public:
	void connect( _Delegate delegate )
	{
		_delegates.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)() )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)() const )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		_delegates.Erase( delegate );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)() )
	{  
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)() const )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	void send() const
	{
		for (int i = 0; i < _delegates.Size(); i++)
		{
			(_delegates[i])();
		}
	}

	void operator() () const
	{
		send();
	}
};

template< class Param1 >
class palSignal1
{
public:
	typedef _pal_delegate_internal::palDelegate1< Param1 > _Delegate;

private:
	palArray<_Delegate> _delegates;
public:
	void connect( _Delegate delegate )
	{
		_delegates.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1 ) )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1 ) const )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		_delegates.Erase( delegate );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1 ) )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1 ) const )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	void send( Param1 p1 ) const
	{
		for (int i = 0; i < _delegates.Size(); i++)
		{
			(_delegates[i])(p1);
		}
	}

	void operator() ( Param1 p1 ) const
	{
		send( p1 );
	}
};


template< class Param1, class Param2 >
class palSignal2
{
public:
	typedef _pal_delegate_internal::palDelegate2< Param1, Param2 > _Delegate;

private:
	palArray<_Delegate> _delegates;

public:
	void connect( _Delegate delegate )
	{
		_delegates.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2 ) )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2 ) const )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		_delegates.Erase( delegate );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2 ) )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2 ) const )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	void send( Param1 p1, Param2 p2 ) const
	{
		for (int i = 0; i < _delegates.Size(); i++)
		{
			(_delegates[i])(p1,p2);
		}
	}

	void operator() ( Param1 p1, Param2 p2 ) const
	{
		send( p1, p2 );
	}
};


template< class Param1, class Param2, class Param3 >
class palSignal3
{
public:
	typedef _pal_delegate_internal::palDelegate3< Param1, Param2, Param3 > _Delegate;

private:
	palArray<_Delegate> _delegates;

public:
	void connect( _Delegate delegate )
	{
		_delegates.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3 ) )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3 ) const )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		_delegates.Erase( delegate );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3 ) )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3 ) const )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	void send( Param1 p1, Param2 p2, Param3 p3 ) const
	{
		for (int i = 0; i < _delegates.Size(); i++)
		{
			(_delegates[i])(p1,p2,p3);
		}
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3 ) const
	{
		send( p1, p2, p3 );
	}
};


template< class Param1, class Param2, class Param3, class Param4 >
class palSignal4
{
public:
	typedef _pal_delegate_internal::palDelegate4< Param1, Param2, Param3, Param4 > _Delegate;

private:
	palArray<_Delegate> _delegates;

public:
	void connect( _Delegate delegate )
	{
		_delegates.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) const )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		_delegates.Erase( delegate );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) const )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	void send( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) const
	{
		for (int i = 0; i < _delegates.Size(); i++)
		{
			(_delegates[i])(p1,p2,p3,p4);
		}
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) const
	{
		send( p1, p2, p3, p4 );
	}
};


template< class Param1, class Param2, class Param3, class Param4, class Param5 >
class palSignal5
{
public:
	typedef _pal_delegate_internal::palDelegate5< Param1, Param2, Param3, Param4, Param5 > _Delegate;

private:
	palArray<_Delegate> _delegates;

public:
	void connect( _Delegate delegate )
	{
		_delegates.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) const )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		_delegates.Erase( delegate );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) const )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	void send( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) const
	{
		for (int i = 0; i < _delegates.Size(); i++)
		{
			(_delegates[i])(p1,p2,p3,p4,p5);
		}
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) const
	{
		send( p1, p2, p3, p4, p5 );
	}
};


template< class Param1, class Param2, class Param3, class Param4, class Param5, class Param6 >
class palSignal6
{
public:
	typedef _pal_delegate_internal::palDelegate6< Param1, Param2, Param3, Param4, Param5, Param6 > _Delegate;

private:
	palArray<_Delegate> _delegates;

public:
	void connect( _Delegate delegate )
	{
		_delegates.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) const )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		_delegates.Erase( delegate );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) const )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	void send( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) const
	{
		for (int i = 0; i < _delegates.Size(); i++)
		{
			(_delegates[i])(p1,p2,p3,p4,p5,p6);
		}
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) const
	{
		send( p1, p2, p3, p4, p5, p6 );
	}
};


template< class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7 >
class palSignal7
{
public:
	typedef _pal_delegate_internal::palDelegate7< Param1, Param2, Param3, Param4, Param5, Param6, Param7 > _Delegate;

private:
	palArray<_Delegate> _delegates;
public:
	void connect( _Delegate delegate )
	{
		_delegates.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) const )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		_delegates.Erase( delegate );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) const )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	void send( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) const
	{
		for (int i = 0; i < _delegates.Size(); i++)
		{
			(_delegates[i])(p1,p2,p3,p4,p5,p6,p7);
		}
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) const
	{
		send( p1, p2, p3, p4, p5, p6, p7 );
	}
};


template< class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class Param8 >
class palSignal8
{
public:
	typedef _pal_delegate_internal::palDelegate8< Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8 > _Delegate;

private:
	palArray<_Delegate> _delegates;

public:
	void connect( _Delegate delegate )
	{
		_delegates.push_back( delegate );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void connect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) const )
	{
		_delegates.push_back( palMakeDelegate( obj, func ) );
	}

	void disconnect( _Delegate delegate )
	{
		_delegates.Erase( delegate );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	template< class X, class Y >
	void disconnect( Y * obj, void (X::*func)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) const )
	{
		_delegates.Erase( palMakeDelegate( obj, func ) );
	}

	void send( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) const
	{
		for (int i = 0; i < _delegates.Size(); i++)
		{
			(_delegates[i])(p1,p2,p3,p4,p5,p6,p7,p8);
		}
	}

	void operator() ( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) const
	{
		send( p1, p2, p3, p4, p5, p6, p7, p8 );
	}
};

#endif