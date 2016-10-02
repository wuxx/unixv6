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
	char *m_size;	/* ��ĸ��� */
	char *m_addr;	/* ��ʼ�����ţ�(ÿ���СΪ64 Bytes) */
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
		if (bp->m_size >= size) {	/* ��map���ҵ�һ�����ʵ�bp */
			a = bp->m_addr;
			bp->m_addr =+ size;
			if ((bp->m_size =- size) == 0)	/* ���bp��¼���ڴ��С���õ�����Ҫ���ڴ��С�����¼������ǰƽ�� */
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
mfree(mp, size, aa)	/* size Ϊ��ĸ�����aaΪ��ʼ��� */
struct map *mp;		/* mp ���ǰ��տ�ŵ�����˳���¼�����Ϣ�� */
{
	register struct map *bp;
	register int t;
	register int a;

	a = aa;
	for (bp = mp; bp->m_addr<=a && bp->m_size!=0; bp++);	/* ��ȡ��һ�����õ�bp */
	if (bp>mp && (bp-1)->m_addr+(bp-1)->m_size == a) {		/* ���Ժ�bp-1 �����¼�γ���������ϲ���bp-1�� */
		(bp-1)->m_size =+ size;	/* ����bp-1 ��¼�еĿ���� */
		if (a+size == bp->m_addr) {	/* �ϲ���bp-1��֮�󣬷��ֺ�bp ��¼Ҳ�γ������� */
			(bp-1)->m_size =+ bp->m_size;	/* ���ٴθ���bp-1�еĿ�ĸ��� */
			while (bp->m_size) {			/* bp����ۿ����ˣ�����֮��ļ�¼��ǰƽ�� */
				bp++;
				(bp-1)->m_addr = bp->m_addr;
				(bp-1)->m_size = bp->m_size;
			}
		}
	} else {
		if (a+size == bp->m_addr && bp->m_size) {	/* ���Ժ�bp�����¼�γ����� */
			bp->m_addr =- size;
			bp->m_size =+ size;
		} else if (size) do {	/* �Ȳ��ܺ�ǰһ���¼�������ֲ��ܺͺ�һ���γ���������ռ��bp�����¼������bp��ʼ�ļ�¼����ƽ�� */
			t = bp->m_addr;		/* ��ʱ����bp��¼����ʼ��� */
			bp->m_addr = a;
			a = t;
			t = bp->m_size;
			bp->m_size = size;
			bp++;
		} while (size = t);		/* ��һ��ִ����֮��bp�Ѿ��洢���µ���������Ϣ��bpԭ������Ϣ����a��size�������a��size��������ʱ�洢��ռ�õĲ۵���Ϣ */
	}
}
