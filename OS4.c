#include "OS4.h"

frame_node *Create_Node(int value) // 프레임에 들어갈 새로운 노드 생성
{
    frame_node *temp = (frame_node*)malloc(sizeof(frame_node)); // 노드 생성
    temp->value = value; // 입력 값 저장
    temp->lfu_index=0;
    temp->lru_index=0;
    temp->next = NULL;
    temp->prev = NULL;
    temp->optimal_index = -1; //아직 값이 정해지지 않은 상태
    temp->sc_bool = 0; // 이 값이 1일 경우 한번더 기회를 가질 수 있다.
    return temp;
}

void Link_first_node(frame_link *link, frame_node *node) // frame에 첫번째 노드가 들어갈 경우 head와 tail을 모두 연결하는 역할
{
    link->head = node;
    link->tail = node;
    link->sz++; // frame에 노드가 추가로 들어가기에 size 증가
}

void Insert_tail_node(frame_link *link, frame_node *node) // frame에 추가적으로 노드를 삽입할 때 불러오는 함수
{
    link->tail->next = node;
    node->prev = link->tail;
    link->tail = node;
    link->sz++; // frame에 노드가 추가로 들어가기에 size 증가
}

frame_node *find_node(frame_link *link, int value) // frame 내부에 value에 맞는 노드가 있는지 확인 있으면 노드 반환.
{
    frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
    temp_node = link->head; // frame의 첫 노드로 선정
    while(1)
    {
        if(temp_node->value == value) // 동일한 값이 있을 경우
        {
            return temp_node; // 해당 노드 반환
        }
        if(temp_node->next == NULL) // 없을 경우
        {
            return NULL; // NULL 반환
        }
        temp_node = temp_node->next; // 반복
    }
}

int make_opt_index(page_string_node *string_node, int start_cnt, int value) // optimal 알고리즘 사용시, 확인하고 있는 값이 얼마 뒤에 나타내는지 구하는 함수
{
    page_string_node *temp_node = (page_string_node*)malloc(sizeof(page_string_node));
    int cnt = start_cnt; // 시작 index
    temp_node = string_node;
    if(temp_node->next == NULL) // page string에서 마지막 값일 경우
    {
        return 1e+7;
    }
    while(1)
    {
        temp_node = temp_node->next; // 반복
        cnt++; // index 증가
        if(temp_node->value == value) // 동일한 값을 찾은 경우 그 위치값을 반환
        {
            return cnt;
        }
        if(temp_node->next == NULL) // page string에서 더이상 동알헌 값이 등장하지 않을때,
        {
            return 1e+7;
        }
    }
}

frame_node *find_opt_node(frame_link *link) // optimal page frame 교체시 교체해야 하는 노드를 찾아내서 반환하는 함수
{
    frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
    frame_node *save_node = (frame_node*)malloc(sizeof(frame_node));
    temp_node = link->head;
    int index = -1;
    while(1)
    {
        if(index < temp_node->optimal_index) // 기존의 index보다 더 큰 값이 존재하는 경우
        {
            index = temp_node->optimal_index; // index 업데이트
            save_node = temp_node; // 업데이트된 인덱스의 노드 지정
        }
        if(temp_node->next == NULL) // 마지막까지 탐색
        {
            return save_node; // 가장 큰 index를 가진 노드 반환
        }
        temp_node = temp_node->next;
    }
}

frame_node *find_lru_node(frame_link *link) // lru 방법 사용시 교체해야 하는 노드를 찾아내서 반환하는 함수
{
    frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
    frame_node *save_node = (frame_node*)malloc(sizeof(frame_node));
    int index=1e+9;
    temp_node = link->head;
    while(1)
    {
        if(index > temp_node->lru_index) // 기존의 index보다 더 작은 값이 존재할 경우
        {
            index = temp_node->lru_index; // index 업데이트
            save_node = temp_node; // 업데이트된 인덱스의 노드 지정
        }
        if(temp_node->next == NULL) // 마지막까지 탐색
        {
            return save_node; // 가장 작은 index를 가진 노드 반환
        }
        temp_node = temp_node->next;
    }
}

frame_node *find_lfu_node(frame_link *link) // lfu 방법 사용시 교체해야 하는 노드를 찾아내서 반환하는 함수
{
    frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
    frame_node *save_node = (frame_node*)malloc(sizeof(frame_node));
    int lfu_index = 1e+9; // lfu_index 비교를 위해
    int lru_index = 1e+9; // lru_index 비교를 위해
    char same_lfu_index_bool=0; // 가장 작은 lfu index가 2개 이상있는 경우
    temp_node = link->head;
    while(1)
    {
        if(lfu_index > temp_node->lfu_index)  // 기존의 index보다 더 작은 값이 존재할 경우
        {
            lfu_index = temp_node->lfu_index; // index 업데이트
            save_node = temp_node; // 업데이트된 인덱스의 노드 지정
            same_lfu_index_bool = 0; // 다시 0으로 업데이트
        }
        else if(lfu_index == temp_node->lfu_index)
        {
            same_lfu_index_bool = 1; // 동일한 값이 존재하므로 1로 변경
        }
        if(temp_node->next == NULL) // 마지막까지 탐색
        {
            break;
        }
        temp_node = temp_node->next;
    }
    if(same_lfu_index_bool == 1) // 동일한 lfu index가 2개 이상인 경우
    {   
        temp_node = link->head;
        while(1)
        {
            if(lfu_index == temp_node->lfu_index && lru_index > temp_node->lru_index) // lru index 비교를 통해 노드 탐색
            {
                lru_index = temp_node->lru_index;
                save_node = temp_node;
            }
            if(temp_node->next==NULL)
            {
                break;
            }
            temp_node = temp_node->next;
        }
    }
    return save_node;
}

int find_value(frame_link *link, int value) // frame 내부에 찾는 값이 있는지 확인하는 함수. 있으면 1 반환, 없으면 0 반환
{
    frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
    temp_node = link->head;
    while(1)
    {
        if(temp_node->value == value) // 찾는 값이 frame 내부에 존재하는 경우
        {
            return 1;
        }
        if(temp_node->next == NULL) // 찾는 값이 frame 내부에 없는 경우
        {
            return 0;
        }
        temp_node = temp_node->next;
    }
}

void swap_node(frame_link *link, frame_node *temp_node, frame_node *node) // 노드 교체를 위한 함수
{
    if(temp_node->value == link->head->value) // 교체 노드가 head인 경우
    {
        node->next = temp_node->next;
        temp_node->next->prev = node;
        link->head = node;
    }
    else if(temp_node->value == link->tail->value) // 교체 노드가 tail인 경우
    {
        node->prev = temp_node->prev;
        temp_node->prev->next = node;
        link->tail = node;
    }
    else // 그외
    {
        node->prev = temp_node->prev;
        node->next = temp_node->next;
        temp_node->next->prev = node;
        temp_node->prev->next = node;
    }
    free(temp_node);
}

void method_output(int mode) // 각각의 방법에 대한 출력
{
    printf("Used method : ");
    fprintf(output_f, "Used method : ");
    if(mode == 0)
    {
        printf("OPT\n");
        fprintf(output_f, "OPT\n");
    }
    else if(mode == 1)
    {
        printf("FIFO\n");
        fprintf(output_f, "FIFO\n");
    }
    else if(mode == 2)
    {
        printf("LIFO\n");
        fprintf(output_f, "LIFO\n");
    }
    else if(mode == 3)
    {
        printf("LRU\n");
        fprintf(output_f, "LRU\n");
    }
    else if(mode == 4)
    {
        printf("LFU\n");
        fprintf(output_f, "LFU\n");
    }
    else if(mode == 5)
    {
        printf("SC\n");
        fprintf(output_f, "SC\n");
    }
}

void print_page_string(page_string_node *start_node) // page_reference_string의 모든 값을 출력
{
    printf("page reference string : ");
    fprintf(output_f, "page reference string : ");
    temp_node = start_node;
    while(1) // 반복을 하며, 모든 page_reference_string의 값을 순서대로 출력
    {
        printf("%d ", temp_node->value);
        fprintf(output_f, "%d ", temp_node->value);
        if(temp_node->next == NULL)
        {
            break;
        }
        temp_node = temp_node->next;
    }
    printf("\n");
    fprintf(output_f, "\n");
}

void frame_output(int mode, frame_link *link) // 출력의 틀을 생성
{
    int i, j;
    int cnt=0; // page string의 반복횟수 == page string의 index
    int cnt_page_fault = 0; // page fault가 일어난 횟수를 저장한 변수
    int value; // page string의 구조체에 있는 값을 불러와 저장하는 변수
    temp_node = (page_string_node*)malloc(sizeof(page_string_node));
    printf("%10s", "Frame");
    fprintf(output_f, "%10s", "Frame");
    for(i=0; i<frame_num; i++)
    {
        printf("%5d", i+1);
        fprintf(output_f, "%5d", i+1);
    }
    printf("%15s", "page falut");
    printf("\n");
    printf("time\n");
    fprintf(output_f, "%15s", "page falut");
    fprintf(output_f, "\n");
    fprintf(output_f, "time\n");
    temp_node = start_node;
    while(1) // mode 값에 맞는 알고리즘 시행
    {
        bool_page_fault = 0;
        printf("%-10d", cnt+1);
        fprintf(output_f, "%-10d", cnt+1);
        value = temp_node->value;
        if(mode==0)
        {
            OPT(temp_node, link, cnt);
            print_num(link);
        }
        else if(mode==1)
        {
            FIFO(value, link);
            print_num(link);
        }
        else if(mode==2)
        {
            LIFO(value, link);
            print_num(link);
        }
        else if(mode==3)
        {
            LRU(value, link, cnt);
            print_num(link);
        }
        else if(mode==4)
        {
            LFU(value, link, cnt);
            print_num(link);
        }
        else if(mode==5)
        {
            Second_Chance(value, link);
            print_num(link);
        }
        if(bool_page_fault==1) // page fault 발생 시
        {
            printf("%6c", 'F');
            fprintf(output_f, "%6c", 'F');
            cnt_page_fault++; // page fault가 발생했기에 전체 횟수 증가
        }
        printf("\n");
        fprintf(output_f, "\n");
        cnt++;
        if(temp_node->next == NULL) // 마지막 page string까지 온 경우
        {
            break;
        }
        temp_node = temp_node->next;
    }
    printf("Number of Page Faults : %d times\n", cnt_page_fault);
    printf("hit rate: %f\n", (double)(num_reference_string-cnt_page_fault) / num_reference_string*100.);
    printf("\n");

    fprintf(output_f, "Number of Page Faults : %d times\n", cnt_page_fault);
    fprintf(output_f, "hit rate: %f\n", (double)(num_reference_string-cnt_page_fault) / num_reference_string*100.);
    fprintf(output_f, "\n");
    //free(temp_node);
}

/*
모든 아래의 알고리즘 함수는 3가지의 if문으로 나누어집니다.

첫번째는 frame의 사이즈가 0인경우 즉, frame에 아무것도 들어있지 않은 상황인 경우입니다.
이 경우 Link_first_node 함수를 호출하여 frame의 head와 tail을 첫번째 node로 설정합니다.
또한 무조건 page fault를 발생시킵니다.

두번째 경우는 frame의 사이즈가 frame_num(전체 frame의 크기)와 다른 경우입니다.
이 경우, 알고리즘에 따라 입력되고 있는 값이 frame 내부에 있는지 없는지 조사합니다. 
공통적으로 없다면, Insert_tail_node 함수를 호출하여 frame list tail에 연결하며, page fault를 발생시킵니다.
내부에 있다면, 알고리즘에 따라 아무것도 안할 때도 있고, 어떠한 과정을 거칠 수도 있습니다.

마지막 경우는 frame의 사이즈가 frame_num가 같은 경우로 이 경우는 frame에 모든 값이 들어가있는 경우입니다.
위와 마찬가지로 입력된 값이 frame 내부에 있는지 조사하여 없다면 swap_node 함수를 통해 기존의 노드를 제거하고 그 자리에 새로운 노드를 연결합니다.
이 경우 page fault를 발생시킵니다.
내부에 이미 존재한다면 마찬가지로 알고리즘에 따라 과정을 거칩니다.
*/

void OPT(page_string_node *string_node, frame_link *link, int index) // optimal 알고리즘을 실행하는 함수
{
    node = Create_Node(string_node->value); // 입력 노드 생성
    if(link->sz == 0) // frame이 비어있다면
    {
        node->optimal_index = make_opt_index(string_node, index, node->value); // 연결할 노드의 opt index 입력
        Link_first_node(link, node); // 첫 노드 연결
        bool_page_fault=1; // page fault 발생
    }
    else if(link->sz != frame_num) // frame이 완전히 차있지 않다면
    {
        if(find_value(link, node->value)==0) // 입력 노드의 값이 frame에 없다면
        {
            node->optimal_index = make_opt_index(string_node, index, node->value); // 연결할 노드의 opt index 입력
            Insert_tail_node(link, node); // frame list tail에 노드 연결
            bool_page_fault=1; // page fault 발생
        }
        else // 입력 노드에 이미 존재한다면
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node)); // 임시 노드 생성
            int optimal_index = make_opt_index(string_node, index, node->value); // 업데이트할 노드의 opt index 입력
            temp_node = find_node(link, node->value); // opt index 업데이트할 노드 찾기
            temp_node->optimal_index = optimal_index; // opt index 업데이트
        }
    }
    else // frame의 노드가 다 있다면
    {
        if(find_value(link, node->value)==0) // 입력 노드의 값이 frame에 없을시
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node)); // 임시 노드 생성
            node->optimal_index = make_opt_index(string_node, index, node->value); // 연결할 노드의 opt index 입력
            temp_node = find_opt_node(link); // 가장 낮은 opt index의 노드 찾기
            swap_node(link, temp_node, node); // frame에서 노드 교체
            bool_page_fault=1; // page fault 발생
        }
        else // 입력 노드의 값이 frame에 있을시
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node)); // 임시 노드 생성
            int optimal_index = make_opt_index(string_node, index, node->value); // 업데이트할 노드의 opt index 입력
            temp_node = find_node(link, node->value); // opt index 업데이트할 노드 찾기
            temp_node->optimal_index = optimal_index; // opt index 업데이트
        }
    }
}

void FIFO(int value, frame_link *link) // FIFO 알고리즘을 실행하는 함수
{
    node = Create_Node(value); // 입력 노드 생성
    if(link->sz == 0) // frame이 비어있다면
    {
        Link_first_node(link, node); // 첫 노드 연결
        bool_page_fault=1; // page fault 발생
    }
    else if(link->sz != frame_num) // frame이 완전히 차있지 않다면
    {
        if(find_value(link, value)==0) // 입력 노드의 값이 frame에 없다면
        {
            Insert_tail_node(link, node); // frame list tail에 노드 연결
            bool_page_fault=1; // page fault 발생
        }
    }
    else // frame의 노드가 다 있다면
    {
        if(find_value(link, value)==0) // 입력 노드의 값이 frame에 없을시
        {
            int cnt=0; // 반복 횟수
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node)); // 임시 노드 생성
            temp_node = link->head;
            while(1) // 임시 노드를 head로 하여 FIFO_queue와 같은 값을 가질때까지 cnt 증가
            {
                if(cnt==FIFO_queue)
                {
                    swap_node(link, temp_node, node); // 노드 교체
                    FIFO_queue++;
                    bool_page_fault=1; // page fault 발생
                    break;
                }
                temp_node = temp_node->next;
                cnt++;
            }
        }
    }
    FIFO_queue = FIFO_queue % frame_num; // FIFO_queue circular queue처럼 사용 
}

void LIFO(int value, frame_link *link) // LIFO 알고리즘을 실행하는 함수
{
    node = Create_Node(value); // 입력 노드 생성
    if(link->sz==0) // frame이 비어있다면
    {
        Link_first_node(link, node); // 첫 노드 연결
        lifo_first = node; // 가장 먼저 제거할 노드로 선정
        bool_page_fault=1; // page fault 발생
    }
    else if(link->sz!=frame_num) // frame이 완전히 차있지 않다면
    {
        if(find_value(link, value)==0) // 입력 노드의 값이 frame에 없다면
        {
            Insert_tail_node(link, node); // frame list tail에 노드 연결
            lifo_first = node; // 가장 먼저 제거할 노드로 선정
            bool_page_fault=1; // page fault 발생
        }
    }
    else // frame의 노드가 다 있다면
    {
        if(find_value(link, value)==0) // 입력 노드의 값이 frame에 없을시
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node)); // 임시 노드 생성
            temp_node = lifo_first; // 가장 먼저 제거할 노드를 가져오기
            swap_node(link, temp_node, node); // 노드 교체
            lifo_first = node; // 교체한 노드를 가장 먼저 제거할 노드로 선정
            bool_page_fault=1; // page fault 발생
        }
    }
}

// ***여기서부터 위의 알고리즘과 공통된 부분은 주석을 생략하겠습니다.*** //
void LRU(int value, frame_link *link, int index) // LRU 알고리즘을 실행하는 함수
{
    node = Create_Node(value);
    node->lru_index = index; // 생성된 노드의 lru index를 입력된 위치값으로 업데이트
    if(link->sz == 0)
    {
        Link_first_node(link, node);
        bool_page_fault=1;
    }
    else if(link->sz != frame_num)
    {
        if(find_value(link, value)==0)
        {
            Insert_tail_node(link, node);
            bool_page_fault=1;
        }
        else
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
            temp_node = find_node(link, value);
            temp_node->lru_index = index; // 새롭게 등장한 동일한 값이 등장하였기 때문에 lru_index 업데이트
        }
    }
    else
    {
        if(find_value(link, value) == 0)
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
            temp_node = find_lru_node(link); // 가장 낮은 값의 lru_index를 가진 노드를 찾기
            swap_node(link, temp_node, node);
            bool_page_fault=1;
        }
        if(find_value(link, value)==1)
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
            temp_node = find_node(link, value);
            temp_node->lru_index = index; // 새롭게 등장한 동일한 값이 등장하였기 때문에 lru_index 업데이트
        }
    }
}

void LFU(int value, frame_link *link, int index) // LFU 알고리즘을 실행하는 함수
{
    node = Create_Node(value);
    node->lru_index = index; // lru_index도 함께 지정
    node->lfu_index = 1; // 참조횟수를 지정하기 때문에, 처음 생성된 노드의 lfu_index의 값은 1로 설정
    if(link->sz==0)
    {
        Link_first_node(link, node);
        bool_page_fault=1;
    }
    else if(link->sz != frame_num)
    {
        if(find_value(link, value)==1)
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
            temp_node = find_node(link, value);
            temp_node->lfu_index++; // 동일한 값의 노드가 이미 존재하기 때문에, 그 노드의 lfu_index의 값을 증가시킴
            temp_node->lru_index = index;
        }
        else
        {
            Insert_tail_node(link, node);
            bool_page_fault=1;
        }
    }
    else
    {
        if(find_value(link, value)==1)
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
            temp_node = find_node(link, value);
            temp_node->lfu_index++; // 동일한 값의 노드가 이미 존재하기 때문에, 그 노드의 lfu_index의 값을 증가시킴
            temp_node->lru_index = index;
        }
        else
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
            temp_node = find_lfu_node(link); // 가장 적게 참조된 노드를 찾아서 반환함
            swap_node(link, temp_node, node);
            bool_page_fault=1;
        }
    }
}

void Second_Chance(int value, frame_link *link) // SC 알고리즘을 실행하는 함수
{
    frame_node *node = Create_Node(value); // SC의 경우 생성함수 내부에서 관련 변수의 값을 초기화함 (sc_bool = 0)
    if(link->sz==0)
    {
        Link_first_node(link, node);
        bool_page_fault=1;
        save_sc_node = link->head; // save_sc_node: circular queue의 역할
    }
    else if(link->sz != frame_num)
    {
        if(find_value(link, value)==0)
        {
            Insert_tail_node(link, node);
            bool_page_fault=1;
        }
        else
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
            temp_node = find_node(link, value);
            temp_node->sc_bool=1; // 동일한 값이 등장하였기 때문에 1로 변경하여 한번더 기회를 준다.
        }
    }
    else
    {
        if(find_value(link, value)==0)
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
            temp_node = save_sc_node;
            while(1)
            {
                if(temp_node->sc_bool==0) //  sc_bool의 값이 0인 노드를 발견한 경우
                {
                    swap_node(link, temp_node, node); //노드 교체
                    if(node->next == NULL) // frame의 끝에 온 경우
                    {
                        save_sc_node = link->head; // 처음의 frame의 노드로 지정
                    }
                    else
                    {
                        save_sc_node = node->next; // circular queue의 노드를 교체한 노드의 다음 노드로 지정
                    }
                    bool_page_fault=1;
                    break;
                }
                else
                {
                    temp_node->sc_bool = 0; // sc_bool의 값이 1이면, 0으로 교체
                }

                if(temp_node->next==NULL) // 다음 노드가 없을 경우, 처음의 frame 노드로 초기화
                {
                    temp_node = link->head;
                }
                else
                {
                    temp_node = temp_node->next; // 노드 탐색 반복
                }
            }
        }
        else
        {
            frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
            temp_node = find_node(link, value);
            temp_node->sc_bool=1; // 동일한 값이 등장하였기 때문에 1로 변경하여 한번더 기회를 준다.
        }
    }
}

void print_num(frame_link *link) // frame에 들어간 값들을 출력
{
    int i;
    frame_node *temp_node = (frame_node*)malloc(sizeof(frame_node));
    temp_node = link->head;
    while(1) // 현재 frame 안에 있는 노드들을 반복하면서 출력
    {
        printf("%5d", temp_node->value);
        fprintf(output_f, "%5d", temp_node->value);
        if(temp_node->next==NULL)
        {
            break;
        }
        temp_node = temp_node->next;
    }
    for(i=0; i<frame_num-link->sz; i++)
    {
        printf("%5c", ' ');
        fprintf(output_f, "%5c", ' ');
    }
}

void make_random() // random으로 page string 생성
{
    int i;
    int random_number = rand()%1000 + 500; //최소 갯수는 500 최개 갯수는 1000
    temp_node = (page_string_node*)malloc(sizeof(page_string_node));
    start_node = (page_string_node*)malloc(sizeof(page_string_node));
    end_node = (page_string_node*)malloc(sizeof(page_string_node));
    for(i=0; i<random_number; i++)
    {
        page_string_node *node = (page_string_node*)malloc(sizeof(page_string_node));
        node->value = rand() % 30 + 1; // 1~30까지의 범위
        node->next = NULL;
        node->prev = NULL;
        if(i==0) // 첫 노드일 경우ㄴ
        {
            start_node = node;
            end_node = node;
        }
        else // 그 외
        {
            end_node->next = node;
            node->prev = end_node;
            end_node = node;

        }
        num_reference_string++;
    }
}

int file_exists(char *fname) // 파일 존재 유무 확인
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void read_file_and_save_string(char *fname) // 파일의 page string을 읽어 저장하는 함수
{
    FILE *f = fopen(fname, "r"); // 파일 읽기
    temp_node = (page_string_node*)malloc(sizeof(page_string_node));
    start_node = (page_string_node*)malloc(sizeof(page_string_node));
    end_node = (page_string_node*)malloc(sizeof(page_string_node));
    while(fgets(buffer, sizeof(buffer), f)) // 끝까지 반복
    {
        char *ptr = strtok(buffer, " ");
        while(ptr != NULL)
        {
            page_string_node *node = (page_string_node*)malloc(sizeof(page_string_node));
            node->value = atoi(ptr); // int형 변환
            node->next = NULL;
            if(num_reference_string == 0)
            {
                start_node = node;
                end_node = node;
            }
            else
            {
                end_node->next = node;
                node->prev = end_node;
                end_node = node;
            }
            ptr = strtok(NULL, " ");
            num_reference_string++;
        }
    }
    fclose(f);
}

void make_file() // file을 생성하여 page string 저장 및 생성
{
    char fname[32];
    int i;
    printf("파일 이름을 입력해주세요.(파일이 없을 경우 파일명으로 생성하여 진행합니다.)\n");
    scanf("%s", fname);
    if(file_exists(fname) == 1) //파일이 존재하는 경우
    {   
        printf("파일이 존재하는 것을 확인했습니다.\n");
        read_file_and_save_string(fname); // 파일 읽기
    }
    else // 파일이 존재하지 않은 경우, 입력 받은 파일 이름으로 생성하여 진행
    {
        printf("파일이 존재하지 않아 생성하여 진행합니다.\n");
        int random_number = random() % 1000 + 500; // 500 ~ 1500
        char s_num[4];
        int i_num;
        FILE *f = fopen(fname, "w");
        for(i=0; i<random_number; i++)
        {
            i_num = random() % 30 + 1; // 1 ~ 30
            sprintf(s_num, "%d", i_num);
            fputs(s_num, f);
            fputs("\n", f);
        }
        fclose(f);
        read_file_and_save_string(fname); // 파일 읽기
    }
}

void free_string_struct(page_string_node *start_node) // page string 내의 노드들 할당 제거
{
    temp_node = start_node;
    while(1)
    {
        start_node = start_node->next;
        free(temp_node);
        temp_node = start_node;
        if(temp_node->next == NULL)
        {
            break;
        }
    }
}

int main(int argc, char *argv[]) // main 함수
{
    int i, j;
    char select_sim_mode[8];
    int mode[6]; // 출력할 알고리즘을 저장하기 위한 배열 1일경우, 해당 알고리즘 시행
    int input_mode; // 입력을 랜덤으로 할지, 파일로 할지 정하기 위한 변수
    int a_bool=0;
    frame_node *save_sc_node = (frame_node*)malloc(sizeof(frame_node));
    frame_node *lifo_first = (frame_node*)malloc(sizeof(frame_node));
    frame_node *lifo_second = (frame_node*)malloc(sizeof(frame_node));
    if(argc >= 2) // 결과 파일을 따로 입력받는 경우
    {
        output_f = fopen(argv[1], "w"); // 입력된 파일로 결과값 저장하기
    }
    else
    {
        output_f = fopen("page_output.txt", "w"); // 디폴트 파일
    }
    printf("A. Page Replacement 알고리즘 시뮬레이터를 선택하십시오 (최대 3개. 넘어가면 자동 제외)\n");
    printf("(All이 포함되면 입력된 숫자와 상관없이 무조건 모든 방법 출력)\n");
    printf(" (1) Optimal  (2) FIFO  (3) LIFO  (4) LRU  (5) LFU  (6) SC (7) All\n");
    while(a_bool == 0)
    {
        memset(mode, 0, sizeof(int)*6);
        fgets(select_sim_mode, 8, stdin); // 문자열로 한번에 입력받기
        for(i=0; i<3; i++) // 최대 3번 반복하기
        {
            int a_value = select_sim_mode[i*2] - '0';
            if(a_value == 7) // 값이 7일 경우
            {
                for(j=0; j<6; j++) // 모든 mode를 출력할 수 있도록 하기
                {
                    mode[j] = 1;
                }
                break;
            }
            else
            {
                mode[a_value-1] = 1;
            }
        }
        a_bool = 1;
    }
    mode[0] = 1; //optimal은 항상 수행될 수 있도록
    printf("B. 페이지 프레임의 개수를 입력하시오.(3~10)\n");
    scanf("%d", &frame_num);
    while(frame_num < 3 || frame_num > 10) // 해당 범위가 아닐 경우 다시 실행
    {
        printf("잘못된 입력입니다. 3~10 사이의 숫자를 입력하세요.\n");
        scanf("%d", &frame_num);
    }
    printf("C. 데이터의 입력 방식을 선택하시오\n");
    printf("(1) 랜덤하게 생성\n");
    printf("(2) 사용자 생성 파일 오픈 \n");
    scanf("%d", &input_mode);
    while(input_mode != 1 && input_mode != 2)
    {
        printf("잘못된 입력입니다. 다시 시도해주세요.\n");
        scanf("%d", &input_mode);
    }

    if(input_mode == 1)
    {
        make_random(); // 무작위로 생성하여 알고리즘 진행
    }
    else if(input_mode == 2)
    {
        make_file(); // 파일로 입력받아 알고리즘 진행
    }

    printf("\n"); // 입력과 격차를 위해
    print_page_string(start_node); // 전체 page string 출력
    for(i=0; i<6; i++) // mode 수만큼 반복하면서
    {
        if(mode[i] == 1) // 해당 mode가 시행되야 한다면
        {
            link = (frame_link*)malloc(sizeof(frame_link)); // frame 초기화
            link->sz=0; // frame 내부의 노드 갯수 0으로 초기화
            method_output(i); // 어떤 알고리즘인지 출력
            printf("\n");
            frame_output(i, link); // 알고리즘 시행
            printf("\n");
            free(link); // 초기화
            
        }
    }
    printf("D. 종료\n");
    free(link);
    free(node);
    free_string_struct(start_node);
    free(temp_node);
    fclose(output_f);
    return 0;
}
