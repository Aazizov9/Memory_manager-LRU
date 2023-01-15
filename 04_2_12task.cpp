#include "os_mem.h"
#include <iostream>

using namespace std;

//mem_handle_t* ram = NULL;

int* ram = NULL;
int* cnt = NULL;
int last; // index v rame poslednogo zapisonnogo elementa
int page_sz, page_cnt;
bool* write_bil = NULL;

int create(int size, int num_pages) {
	if (ram != NULL || size == 0 || num_pages == 0) return 0;
	//ram = (mem_handle_t*)malloc(num_pages * sizeof(mem_handle_t));
	//ram->size = size;
	ram = (int*)malloc(num_pages * sizeof(int));
	cnt = (int*)malloc(num_pages * sizeof(int));
	write_bil = (bool*)malloc(num_pages * sizeof(bool));
	for (int i = 0; i < num_pages; i++) cnt[i] = 0, write_bil[i] = 0, ram[i] = 0;
	page_sz = size;
	page_cnt = num_pages;
	last = -1;
	return 1;
}

int destroy() {
	if (ram == NULL) return 0;
	free(ram); ram = NULL;
	free(cnt); cnt = NULL;
	free(write_bil); write_bil = NULL;
	return 1;
}

int page_num(mem_handle_t block) {
	return (block.addr % page_sz + block.size <= page_sz ? block.addr / page_sz : -1);
}

int read(int addr, int size) {
	if (page_num(mem_handle_t(addr, size)) == -1) return -1;
	//cout << "read::: ";
	//cout << "OK1\n";
	//cout << last << " -- " << sizeof(ram) / sizeof(ram[0]) - 1 << endl;
	for (int i = 0; i <= last; i++) {
		if (ram[i] == addr / page_sz) {
			for (int j = 0; j <= last; j++) ++cnt[j];
			cnt[i] = 0;
			cout << "read " << addr << " " << size << " " << i << endl;  // vozmojno vmesti i last
			return 1;
		}
	} // yesli v ram uje yest etot element to vivod
	if (last < page_cnt - 1) {
		last++;
		ram[last] = addr / page_sz;
		///cout << "load1 " << addr / page_sz << " " << last << endl;
		cout << "load " << addr / page_sz << " " << last << endl;
		cout << "read " << addr << " " << size << " " << last << endl;
		for (int i = 0; i < last; i++) ++cnt[i];
		return 1;
	}
	//int ind = max_element(cnt, cnt + last) - cnt;
	int ind = 0, maxx = cnt[0];
	///cout << ":: ";
	for (int i = 0; i < page_cnt; i++) {
		///cout << ram[i] << " ";
		if (maxx < cnt[i]) {
			maxx = cnt[i];
			ind = i;
		}
	}
	///cout << endl;

	if (write_bil[ind])  cout << "save " << ind << " " << ram[ind] << endl;
	write_bil[ind] = false;
	///cout << "load2 " << addr / page_sz << " " << ind << endl;
	cout << "load " << addr / page_sz << " " << ind << endl;
	ram[ind] = addr / page_sz;
	for (int i = 0; i < page_cnt; i++) ++cnt[i];
	cnt[ind] = 0;
	cout << "read " << addr << " " << size << " " << ind << endl;
	return 1;
}

int write(int addr, int size) {
	if (page_num(mem_handle_t(addr, size)) == -1) return -1;
	///cout << "write::: \n";
	//cout << "last= " << last << " -- " << page_cnt - 1 << endl;
	for (int i = 0; i <= last; i++) {
		if (ram[i] == addr / page_sz) {
			for (int j = 0; j <= last; j++) ++cnt[j];
			cnt[i] = 0;
			write_bil[i] = true;
			cout << "write " << addr << " " << size << " " << i << endl;  // vozmojno vmesti i last
			return 1;
		}
	} // yesli v ram uje yest etot element to vivodm yego
	if (last < page_cnt - 1) {
		last++;
		ram[last] = addr / page_sz;
		write_bil[last] = true;
		///cout << "load3 " << addr / page_sz << " " << last << endl;
		cout << "load " << addr / page_sz << " " << last << endl;
		cout << "write " << addr << " " << size << " " << last << endl;
		for (int i = 0; i < last; i++) ++cnt[i];
		return 1;
	}
	//int ind = max_element(cnt, cnt + last) - cnt;
	int ind = 0, maxx = cnt[0];
	for (int i = 0; i < page_cnt; i++) {
		if (maxx < cnt[i]) {
			maxx = cnt[i];
			ind = i;
		}
	}

	if (write_bil[ind])  cout << "save " << ind << " " << ram[ind] << endl;
	write_bil[ind] = true;
	///cout << "load4 " << addr / page_sz << " " << ind << endl;
	cout << "load " << addr / page_sz << " " << ind << endl;
	ram[ind] = addr / page_sz;
	for (int i = 0; i < page_cnt; i++) ++cnt[i];
	cnt[ind] = 0;
	cout << "write " << addr << " " << size << " " << ind << endl;
	return 1;
}


void setup_memory_manager(memory_manager_t* mm) {
	mm->create = create;
	mm->destroy = destroy;
	mm->read = read;
	mm->write = write;
	mm->page_num = page_num;
	//mm->save_page = save_page;    save <real_page> <old_page>
	//mm->load_page = load_page;    load <real_page> <old_page>
}