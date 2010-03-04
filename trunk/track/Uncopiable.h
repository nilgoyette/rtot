
#ifndef __UNCOPIABLE_H__
#define __UNCOPIABLE_H__

class __EmptyBase {};

template <class B = __EmptyBase>
class Uncopiable : B {
	public:
		Uncopiable (const Uncopiable &);
		Uncopiable& operator=(const Uncopiable&);

	protected:
		Uncopiable () throw() {}
		~Uncopiable () throw() {}
};

#endif
