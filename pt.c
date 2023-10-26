#include "os.h"

#define DEPTH (4)
#define SYMBOL_SIZE (9)
#define OFFSET_SIZE (12)
#define SYMBOL_MASK (0x1ff)

int get_next_symbol(uint64_t p, int i);
int is_bit_valid(uint64_t p);

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn) {
	int i;
    uint64_t * node;
    uint64_t curr_symbol;

	node = phys_to_virt(pt << OFFSET_SIZE);

    for (i = DEPTH; i > 0 ; i -= 1){
        curr_symbol = get_next_symbol(vpn, i);
        if(is_bit_valid(node[curr_symbol]) && ppn == NO_MAPPING) {          // no need to destroy
			return;
		}
        if(is_bit_valid(node[curr_symbol]) && ppn != NO_MAPPING){          // update 
			node[curr_symbol] = (alloc_page_frame() << OFFSET_SIZE) | 1;
        }
        node = phys_to_virt(node[curr_symbol] - 1);                      // advance
    }
    curr_symbol = get_next_symbol(vpn, i);
    if(ppn == NO_MAPPING){
			node[curr_symbol] = 0;                                      // destroy
	}
	else {
			node[curr_symbol] = (ppn << OFFSET_SIZE) | 1;       
	}

}


uint64_t page_table_query(uint64_t pt, uint64_t vpn) {
    uint64_t curr_symbol;
    uint64_t * node;
	int i;
	node = phys_to_virt(pt << OFFSET_SIZE);
	for(i = DEPTH; i >= 0; i -= 1){
		curr_symbol = get_next_symbol(vpn, i);
		if (is_bit_valid(node[curr_symbol])){
			return NO_MAPPING;
		}
		if (i > 0){
			node = phys_to_virt(node[curr_symbol] - 1);
		}
	}
	return (node[curr_symbol] >> OFFSET_SIZE);
}

int get_next_symbol(uint64_t p, int i){
	return (p >> (SYMBOL_SIZE * i)) & SYMBOL_MASK;
}

int is_bit_valid(uint64_t p){
	return (p & 1) == 0;
}
