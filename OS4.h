#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PageStringNode{
    int value;
    struct PageStringNode *next;
    struct PageStringNode *prev;
}page_string_node; // 입력 받은 page string을 링크드 리스트 형태로 불러오기 위한 구조체

typedef struct FrameNode{
    int value; // 입력 노드의 값
    struct FrameNode *prev; // frame에서 현재 노드의 이전 노드
    struct FrameNode *next; // frame에서 현재 노드의 이후 노드
    int optimal_index; // optimal 방법시 이 값과 동일한 값이 나오는 순번
    int lru_index; // lru 알고리즘에서 이 노드가 언제 업데이트되었는디 나타내는 값
    int lfu_index; // lfu 알고리즘에서 몇번 등장하였는지 나타내는 값
    char sc_bool; // second chance 알고리즘에서 한번더 기회가 있는지 없는지를 1과 0으로 나타냄
}frame_node; // frame에 들어갈 노드의 구조체

typedef struct FrameLink{
    struct FrameNode *head; // 가장 먼저 들어간 fifo_node
    struct FrameNode *tail; //가장 마지막으로 들어간 fifo_node
    int sz; // 현재 page에 들어간 사이즈
}frame_link; // frame을 구성할 구조체

page_string_node *temp_node;
page_string_node *start_node;
page_string_node *end_node; // page string을 불러와서 활용하기 위한 변수 선언
frame_node *node;
frame_link *link; // frame

FILE *output_f; // 결과값을 저장할 file
int frame_num; //page frame 크기
int page_reference_string[30] = {0, }; // 입력받은 값의 배열
int num_reference_string = 0; // 입력된 숫자의 갯수
int bool_page_fault = 0; // page falut boolean. 1일 경우 page falut 발생
int FIFO_queue = 0; // FIFO queue
frame_node *lifo_first; // lifo시 교체되는 노드를 나타냄
frame_node *lifo_second; // 사용하지 않음
int SC_queue = 0; // Second_Chance_queue
frame_node *save_sc_node; 
char buffer[32]; //.txt파일의 내용을 담을 변수 


frame_node *Create_Node(int value); // 프레임에 들어갈 새로운 노드 생성
void Link_first_node(frame_link *link, frame_node *node); // frame에 첫번째 노드가 들어갈 경우 head와 tail을 모두 연결하는 역할
void Insert_tail_node(frame_link *link, frame_node *node); // frame에 추가적으로 노드를 삽입할 때 불러오는 함수
frame_node *find_node(frame_link *link, int value); // frame 내부에 value에 맞는 노드가 있는지 확인 있으면 노드 반환.
int make_opt_index(page_string_node *string_node, int start_cnt, int value); // optimal 알고리즘 사용시, 확인하고 있는 값이 얼마 뒤에 나타내는지 구하는 함수
frame_node *find_opt_node(frame_link *link); // optimal page frame 교체시 교체해야 하는 노드를 찾아내서 반환하는 함수
frame_node *find_lru_node(frame_link *link); // lru 방법 사용시 교체해야 하는 노드를 찾아내서 반환하는 함수
frame_node *find_lfu_node(frame_link *link); // lfu 방법 사용시 교체해야 하는 노드를 찾아내서 반환하는 함수
int find_value(frame_link *link, int value); // frame 내부에 찾는 값이 있는지 확인하는 함수. 있으면 1 반환, 없으면 0 반환
void swap_node(frame_link *link, frame_node *temp_node, frame_node *node); // 노드 교체를 위한 함수
void method_output(int mode); // 각각의 방법에 대한 출력
void print_page_string(page_string_node *start_node); // page_reference_string의 모든 값을 출력
void frame_output(int mode, frame_link *link); // 출력의 틀을 생성
void OPT(page_string_node *string_node, frame_link *link, int index); // optimal 알고리즘을 실행하는 함수
void FIFO(int value, frame_link *link); // FIFO 알고리즘을 실행하는 함수
void LIFO(int value, frame_link *link); // LIFO 알고리즘을 실행하는 함수
void LRU(int value, frame_link *link, int index); // LRU 알고리즘을 실행하는 함수
void LFU(int value, frame_link *link, int index); // LFU 알고리즘을 실행하는 함수
void Second_Chance(int value, frame_link *link); // SC 알고리즘을 실행하는 함수
void print_num(frame_link *link); // frame에 들어간 값들을 출력
void make_random(); // random으로 page string 생성
int file_exists(char *fname); // 파일 존재 유무 확인
void read_file_and_save_string(char *fname); // 파일의 page string을 읽어 저장하는 함수
void make_file(); // file을 생성하여 page string 실행
void free_string_struct(page_string_node *start_node); // page string 내의 노드들 모든 할당 제거
int main(int argc, char *argv[]); // main 함수
