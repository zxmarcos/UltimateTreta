#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#define ImplementObjectCounter()		\
	private:							\
		static int __objectsCounter;	\
	public:								\
		static int getObjectCounter() {	\
			return __objectsCounter;	\
		}
#define InitObjectCounter(T)			\
		int T::__objectsCounter = 0
#define IncrementObjectCounter()        \
		__objectsCounter++;
#define DecrementObjectCounter()        \
		__objectsCounter--;

#define safeDelete(ptr)	{	\
	if (ptr) {				\
		delete ptr;			\
		ptr = NULL;			\
	}						\
}

#endif
