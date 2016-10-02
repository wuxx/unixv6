#
/*
 */

/*
 * Structure of the coremap and swapmap
 * arrays. Consists of non-zero count
 * and base address of that many
 * contiguous units.
 * (The coremap unit is 64 bytes,
 * the swapmap unit is 512 bytes)
 * The addresses are increasing and
 * the list is terminated with the
 * first zero count.
 */
struct map
{
	char *m_size;	/* 块的个数 */
	char *m_addr;	/* 起始物理块号，(每块大小为64 Bytes) */
};

/*
 * Allocate size units from the given
 * map. Return the base of the allocated
 * space.
 * Algorithm is first fit.
 */
malloc(mp, size)
struct map *mp;
{
	register int a;
	register struct map *bp;

	for (bp = mp; bp->m_size; bp++) {
		if (bp->m_size >= size) {	/* 在map中找到一个合适的bp */
			a = bp->m_addr;
			bp->m_addr =+ size;
			if ((bp->m_size =- size) == 0)	/* 如果bp记录的内存大小正好等于需要的内存大小，则记录整体向前平移 */
				do {
					bp++;
					(bp-1)->m_addr = bp->m_addr;
				} while ((bp-1)->m_size = bp->m_size);
			return(a);
		}
	}
	return(0);
}

/*
 * Free the previously allocated space aa
 * of size units into the specified map.
 * Sort aa into map and combine on
 * one or both ends if possible.
 */
mfree(mp, size, aa)	/* size 为块的个数，aa为起始块号 */
struct map *mp;		/* mp 中是按照块号递增的顺序记录块的信息的 */
{
	register struct map *bp;
	register int t;
	register int a;

	a = aa;
	for (bp = mp; bp->m_addr<=a && bp->m_size!=0; bp++);	/* 获取到一个可用的bp */
	if (bp>mp && (bp-1)->m_addr+(bp-1)->m_size == a) {		/* 可以和bp-1 这个记录形成连续，则合并到bp-1里 */
		(bp-1)->m_size =+ size;	/* 更新bp-1 记录中的块个数 */
		if (a+size == bp->m_addr) {	/* 合并到bp-1中之后，发现和bp 记录也形成了连续 */
			(bp-1)->m_size =+ bp->m_size;	/* 则再次更新bp-1中的块的个数 */
			while (bp->m_size) {			/* bp这个槽空闲了，所有之后的记录向前平移 */
				bp++;
				(bp-1)->m_addr = bp->m_addr;
				(bp-1)->m_size = bp->m_size;
			}
		}
	} else {
		if (a+size == bp->m_addr && bp->m_size) {	/* 可以和bp这个记录形成连续 */
			bp->m_addr =- size;
			bp->m_size =+ size;
		} else if (size) do {	/* 既不能和前一块记录连续，又不能和后一块形成连续，则占用bp这个记录，所有bp开始的记录往后平移 */
			t = bp->m_addr;		/* 临时保存bp记录的起始块号 */
			bp->m_addr = a;
			a = t;
			t = bp->m_size;
			bp->m_size = size;
			bp++;
		} while (size = t);		/* 第一次执行完之后，bp已经存储了新的这个块的信息，bp原来的信息存在a，size里，接下来a，size会用来临时存储被占用的槽的信息 */
	}
}
