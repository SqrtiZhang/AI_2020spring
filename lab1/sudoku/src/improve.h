#pragma once
#include "sodoku.h"

void change_status(int x, int y, int num);
bool candidate_delte(void);
bool hidden_candidate_delte(bool& delte_num);

bool dfs_imp(void)
{
	if (zero_list.empty())
		return true;//填完所有空格

	sum_node++;
	ZERO z = zero_list.front();//取可选数最少的空格

	
	pop_heap(zero_list.begin(), zero_list.end());
	zero_list.pop_back();//删除该空格

	
	for (int k = 1; k <= 9; k++)
	{
		if((z.avaliable_num[k])==1)//k可以填入空格
		{
			vector<ZERO> before_change=zero_list;//保存状态
			vector<ZERO> change_can;
			change_status(z.x, z.y, k);//将空格处填k，同时修改其他空格的状态
			bool can_conti = candidate_delte();//使用侯选数法来减少状态，返回1代表当前状态不与侯选数法冲突
			if (can_conti)
			{
				sudoku[z.x][z.y] = k;
				bool result = dfs_imp();//继续递归
				if (result)
					return true;
			}
			sudoku[z.x][z.y] = 0;//回溯
			zero_list = before_change;
			make_heap(zero_list.begin(), zero_list.end());
		}
	}

	zero_list.push_back(z);//回溯
	push_heap(zero_list.begin(), zero_list.end());
	

	return false;
}

void change_status(int x, int y, int num)
{
	int block1 = 3 * (x / 3) + (y / 3);//空格所在宫

	for (int i = 0; i < zero_list.size(); i++)//对每个未填的空格维护状态
	{
		ZERO z = zero_list[i];
		int block2 = 3 * (z.x / 3) + (z.y / 3);
		bool flag = false;

		if ((z.x == x) && (z.avaliable_num[num] == 1))//在同一行，且候选数包括num
			flag = true;
		else if ((z.y == y) && (z.avaliable_num[num] == 1))//同列
			flag = true;
		else if ((block1 == block2) && (z.avaliable_num[num] == 1))//同宫
			flag = true;
		else if ((z.y == z.x) && (x == y) && (z.avaliable_num[num] == 1))
			flag = true;
		else if (((z.y + z.x) == 8) && ((x + y) == 8) && (z.avaliable_num[num] == 1))
			flag = true;

		if (flag)//修改状态
		{
			zero_list[i].avaliable_num[num] = 0;
			zero_list[i].avaliable_sum--;
		}
	}

	make_heap(zero_list.begin(), zero_list.end());
}


bool candidate_delte(void)
{
	while(1)
	{
		if (!zero_list.size())//如果通过侯选数填完了
			return true;

		ZERO z = zero_list.front();

		if (z.avaliable_sum > 1)
		{
			bool delte_num = false;
			bool can_f = hidden_candidate_delte(delte_num);
			if (!delte_num)
				return can_f;
			continue;
		}

		if (z.avaliable_sum == 0)//如果没有侯选数
			return false;
		
		if (z.avaliable_sum == 1)//只有一个侯选数
		{
			int l;
			for (l = 1; l <= 9 && z.avaliable_num[l]!=1; l++);	
			pop_heap(zero_list.begin(), zero_list.end());
			zero_list.pop_back();
			change_status(z.x,z.y,l);
			sudoku[z.x][z.y] = l;
		}
	}
}

bool hidden_candidate_delte(bool &delte_num)
{
	int r[9][10] = { 0 }, c[9][10] = { 0 }, g[9][10] = { 0 }, d[2][10] = { 0 };
	int r_num[9] = { 0 }, c_num[9] = { 0 }, g_num[9] = { 0 }, d_num[9] = {0};
	int r_s[9] = { 0 }, c_s[9] = { 0 }, g_s[9] = { 0 }, d_s[2] = { 0 };

	for (auto z : zero_list)
	{
		int x = z.x, y = z.y;
		int block = 3 * (x / 3) + (y / 3);
		r_num[x]++;
		c_num[y]++;
		g_num[block]++;
		if (x == y)
			d_num[0]++;
		if ((x + y) == 8)
			d_num[1]++;

		for (int k = 1; k <= 9; k++)
		{
			if (z.avaliable_num[k])
			{
				r[x][k] = 1;
				c[y][k] = 1;
				g[block][k] = 1;
				if (x == y)
					d[0][k] = 1;
				if ((x + y) == 8)
					d[1][k] = 1;
			}
		}
	}

	for (int i = 0; i < 9; i++)
	{
		for (int j = 1; j <= 9; j++)
			r_s[i] += r[i][j];
		if (r_s[i] < r_num[i])
			return false;

		for (int j = 1; j <= 9; j++)
			c_s[i] += c[i][j];
		if (c_s[i] < c_num[i])
			return false;

		for (int j = 1; j <= 9; j++)
			g_s[i] += g[i][j];
		if (g_s[i] < g_num[i])
			return false;
	}

	for (int i = 1; i <= 9; i++)
	{
		d_s[0] += d[0][i];
		d_s[1] += d[1][i];
	}
	if (d_s[0] < d_num[0])
		return false;
	if (d_s[1] < d_num[1])
		return false;

	int n = -1;
	for (auto z : zero_list)
	{
		n++;
		int x = z.x, y = z.y;
		int block = 3 * (x / 3) + (y / 3);
		
		if (r_s[x] == 1)
		{
			int j = 0;
			for (j = 1; j <= 9 && (!r[x][j]); j++);
			zero_list.erase(zero_list.begin() + n);
			change_status(x, y, j);
			sudoku[x][y] = j;
			delte_num = true;
			return true;
		}

		if (c_s[y] == 1)
		{
			int j = 0;
			for (j = 1; j <= 9 && (!c[j][y]); j++);
			zero_list.erase(zero_list.begin() + n);
			change_status(x, y, j);
			sudoku[x][y] = j;
			delte_num = true;
			return true;
		}
		
		if (g_s[x] == 1)
		{
			int j = 0;
			for (j = 1; j <= 9 && (!g[block][j]); j++);
			zero_list.erase(zero_list.begin() + n);
			change_status(x, y, j);
			sudoku[x][y] = j;
			delte_num = true;
			return true;
		}

		if (d_s[0] == 1)
		{
			int j = 0;
			for (j = 1; j <= 9 && (!d[0][j]); j++);
			zero_list.erase(zero_list.begin() + n);
			change_status(x, y, j);
			sudoku[x][y] = j;
			delte_num = true;
			return true;
		}
		if (d_s[1] == 1)
		{
			int j = 0;
			for (j = 1; j <= 9 && (!d[1][j]); j++);
			zero_list.erase(zero_list.begin() + n);
			change_status(x, y, j);
			sudoku[x][y] = j;
			delte_num = true;
			return true;
		}

	}

	return true;
}

