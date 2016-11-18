#ifndef _SUNXI_NAND_IO_H
#define _SUNXI_NAND_IO_H

#include <linux/io.h>

#ifndef set_wbit
#define set_wbit(a, se)	({	\
	u32 __v = readl((a));	\
	__v |= ((u32)(se));		\
	writel(__v, (a));		\
	__v;					\
})
#endif

#ifndef clr_wbit
#define clr_wbit(a, cl)	({	\
	u32 __v = readl((a));	\
	__v &= ~((u32)(cl));	\
	writel(__v, (a));		\
	__v;					\
})
#endif

#ifndef clrset_wbit
#define clrset_wbit(a, cl, se) ({	\
	u32 __v = readl((a));			\
	__v &= ~((u32)(cl));			\
	__v |= ((u32)(se));				\
	writel(__v, (a));				\
	__v;							\
})
#endif

#endif
