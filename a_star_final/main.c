#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include<string.h>
#include"i686-w64-mingw32/include/SDL2/SDL.h"

#define LARGE_NUMBER 10000
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 600


typedef struct t_NODE{
    unsigned int x,y;
    float f,g,h;
    unsigned int can_go;
    unsigned int opened;
    struct t_NODE *parent;
}t_NODE;

typedef struct t_GRAPH{
    unsigned int rows,cols;
    t_NODE **matrice;
}t_GRAPH;

typedef struct PRIORITY_QUEUE{
    unsigned int size,maxsize;
    t_NODE **arr;   //tableau de references des nodes  //heap sous forme de tab
}PRIORITY_QUEUE;

void push_to_queue(PRIORITY_QUEUE *q, t_NODE *n){

    if(q->maxsize==q->size) return;  //queue plein
    int i=0;

    if(q->size >0){
        for(i=q->size;(q->arr[i/2]->f) > n->f && i/2 >0;i/=2 ){
            q->arr[i]= q->arr[i/2];
        }
    }

    q->arr[i]=n;
    q->size++;
}


t_NODE * pop_from_queue(PRIORITY_QUEUE *q){
    if(q->size<=0) return NULL;

    int i, child_index;
    t_NODE *removed_node, *last_node;

    //fonctionnement basique d'un heap,, on enleve le root et on essaye de restaurer la propriete de heap

    removed_node=q->arr[0]; //le premier est tjr le min
    last_node=q->arr[--q->size];

    if(q->size > 0){
        for(i=0;2*i<q->size-1;i=child_index){
            child_index=2*i+1; //pour i les enfant sont 2*i+1 et 2*i+2
            if((child_index!=(q->size-1)) && (q->arr[child_index]->f >q->arr[child_index+1]->f)){
                child_index++; //on choisit l'aure enfant;
            }
            if(last_node->f > q->arr[child_index]->f){
                q->arr[i]=q->arr[child_index];
            }else break; //rien � changer dans le heap
        }
        q->arr[i]=last_node;
    }
    return removed_node;
}

PRIORITY_QUEUE * create_queue(t_GRAPH * g){
    PRIORITY_QUEUE * q=(PRIORITY_QUEUE*)malloc(sizeof(PRIORITY_QUEUE));
    q->maxsize=g->cols * g->rows;
    q->size=0;
    q->arr=(t_NODE**)malloc(sizeof(t_NODE*)*q->maxsize);
    return q;
}

float heuristic(t_NODE* n1, t_NODE* n2) {
    int x,y;
    x = n1->x - n2->x;
    y = n1->y - n2->y;
    x = (x<0) ? (-1*x) : x;
    y = (y<0) ? (-1*y) : y;
    return x+y;
}

float euclidean_distance(t_NODE* n1, t_NODE* n2) {
    float d,x,y;
    x = (n1->x-n2->x)*(n1->x-n2->x);
    y = (n1->y-n2->y)*(n1->y-n2->y);
    d = sqrtf(x+y);
    return d;
}

void update_all_adjacent_nodes(t_GRAPH *g, t_NODE * n, PRIORITY_QUEUE * q){

    t_NODE * adjacent_node;
    int i,j,x,y;

    for(i=-1;i<2;i++){
        for(j=-1;j<2;j++){
            if(i!= 0 || j!= 0){
                x=n->x+i;
                y=n->y+j;
            }
            if(x>=0 && x<g->rows && y>=0 && y<g->cols){
                adjacent_node=&(g->matrice[x][y]);

                if(!(adjacent_node->opened) || !(adjacent_node->can_go)) continue;

                float gg=n->g + euclidean_distance(adjacent_node,n);
                if(gg < adjacent_node->g || adjacent_node->opened==2){
                    adjacent_node->opened=1;
                    adjacent_node->g=gg;
                    adjacent_node->f=adjacent_node->g + adjacent_node->h;
                    adjacent_node->parent=n;
                    push_to_queue(q,adjacent_node);
                }
            }
        }
    }
}

void define_node(t_NODE * n, int x,int y, int can_go)
{
    n->x=x;
    n->y=y;

    n->f = LARGE_NUMBER;
    n->g = LARGE_NUMBER;
    n->h = 0.0;
    n->can_go = can_go;
    n->opened = 2;
    n->parent = NULL;
}

t_GRAPH * create_graph(int nb_col,int nb_row){
    t_NODE **nodes;
    t_NODE *nodes_row;
    t_NODE node;
    t_GRAPH *g=(t_GRAPH*)malloc(sizeof(t_GRAPH));
    nodes=(t_NODE**)malloc(sizeof(t_NODE*)*nb_row);
    g->rows=nb_row;
    g->cols=nb_col;

    int x,y;
    for(x=0;x<nb_row;x++){
        nodes_row=(t_NODE*)malloc(sizeof(t_NODE)*nb_col);
        nodes[x]=nodes_row;
        for(y=0;y<nb_col;y++){
            define_node(&node,x,y,1);
            nodes_row[y]=node;
        }
    }

    g->matrice=nodes;
    return g;
}

t_NODE * a_star(t_GRAPH * g, int start_x,int start_y,int end_x,int end_y){

    int row,col;
    t_NODE *n,*destination_node;
    PRIORITY_QUEUE *queue;


    //appliquer l'heuristic qui depend de la destination (end_x,end_y)
    destination_node=&(g->matrice[end_x][end_y]);
    for(row=0;row<g->rows;row++){
        for(col=0;col<g->cols;col++){
            n=&(g->matrice[row][col]);
            n->h=heuristic(n,destination_node);
        }
    }

    queue=create_queue(g);

    n=&(g->matrice[start_x][start_y]);
    push_to_queue(queue,n);


    while(n != destination_node && queue->size>0){
        n=pop_from_queue(queue);
        n->opened=0;

        update_all_adjacent_nodes(g,n,queue);
    }
    free(queue->arr);
    free(queue);
    return destination_node;
}
t_NODE *recursiveReverseLL(t_NODE *p)
{
    t_NODE *revHead;
    if (p == NULL || p->parent == NULL)
    {
        return p;
    }

    revHead = recursiveReverseLL(p->parent);
    p->parent->parent = p;
    p->parent = NULL;

    return revHead;
}

t_NODE * reconstruct_path(t_NODE * start, t_NODE * end){
    return recursiveReverseLL(end);
}
void print_graph(t_GRAPH *g){
    int i,j;
        printf("\n");
    for (i=g->rows-1; i>=0; i--) {
        for (j=0; j<g->cols; j++) {
            if(g->matrice[i][j].can_go) printf("[%d,%d]",g->matrice[i][j].x,g->matrice[i][j].y);
            else printf("[*,*]");
        }
        printf("\n");
    }
}


void SDL_exit_with_error(const char* msg){
    SDL_Log("ERREUR : %s > %s\n",msg,SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}
SDL_Rect * sdl_prepare_rectangle(SDL_Renderer *r,int x,int y,int h,int w){
    SDL_Rect *rectangle=(SDL_Rect*)malloc(sizeof(SDL_Rect));
    rectangle->x=x;
    rectangle->y=y;
    rectangle->h=h;
    rectangle->w=w;
    return rectangle;
}
void sdl_draw_graph(t_GRAPH *g, SDL_Renderer *r,int sx,int sy,int ex,int ey){
    int i,j;
    for (i=0; i<g->rows; i++){
        for (j=0; j<g->cols; j++) {
                SDL_Rect rectangle=*(sdl_prepare_rectangle(r,g->matrice[i][j].x*(WINDOW_WIDTH/g->cols),g->matrice[i][j].y*(WINDOW_HEIGHT/g->rows),(WINDOW_HEIGHT/g->rows),(WINDOW_WIDTH/g->cols)));
                if(g->matrice[i][j].can_go){
                    if(g->matrice[i][j].opened==0 || g->matrice[i][j].opened==1){
                        if(SDL_SetRenderDrawColor(r,255,128,128,SDL_ALPHA_OPAQUE)!=0){
                            printf("changing drawing color failed \n");
                        }

                        if(SDL_RenderFillRect(r,&rectangle)!=0){
                            printf("drawing a filled rectangle failed \n");
                        }
                        if(SDL_SetRenderDrawColor(r,50,50,50,255)!=0){
                            printf("changing drawing color failed \n");
                        }

                        if(SDL_RenderDrawRect(r,&rectangle)!=0){
                            printf("drawing a rectangle border failed \n");
                        }
                    }else{
                        if(SDL_SetRenderDrawColor(r,128,128,255,SDL_ALPHA_OPAQUE)!=0){
                        printf("changing drawing color failed \n");
                        }

                        if(SDL_RenderFillRect(r,&rectangle)!=0){
                            printf("drawing a filled rectangle failed \n");
                        }
                        if(SDL_SetRenderDrawColor(r,50,50,50,255)!=0){
                            printf("changing drawing color failed \n");
                        }

                        if(SDL_RenderDrawRect(r,&rectangle)!=0){
                            printf("drawing a rectangle border failed \n");
                        }
                    }
                }
                else{
                    if(SDL_SetRenderDrawColor(r,10,10,100,SDL_ALPHA_OPAQUE)!=0){
                        printf("changing drawing color failed \n");
                    }

                    if(SDL_RenderFillRect(r,&rectangle)!=0){
                        printf("drawing a filled rectangle failed \n");
                    }
                    if(SDL_SetRenderDrawColor(r,50,50,50,255)!=0){
                        printf("changing drawing color failed \n");
                    }

                    if(SDL_RenderDrawRect(r,&rectangle)!=0){
                        printf("drawing a rectangle border failed \n");
                    }
                }
                if(i==sx && j==sy){
                    SDL_Rect rct = *(sdl_prepare_rectangle(r,sx*(WINDOW_WIDTH/g->cols),sy*(WINDOW_HEIGHT/g->rows),(WINDOW_HEIGHT/g->rows),(WINDOW_WIDTH/g->cols)));
                    if(SDL_SetRenderDrawColor(r,142,236,125,SDL_ALPHA_OPAQUE)!=0){
                        printf("changing drawing color failed \n");
                    }

                    if(SDL_RenderFillRect(r,&rct)!=0){
                        printf("drawing a filled rectangle failed \n");
                    }
                }
                if(i==ex && j==ey){
                    SDL_Rect rct = *(sdl_prepare_rectangle(r,ex*(WINDOW_WIDTH/g->cols),ey*(WINDOW_HEIGHT/g->rows),(WINDOW_HEIGHT/g->rows),(WINDOW_WIDTH/g->cols)));
                    if(SDL_SetRenderDrawColor(r,255,248,106,SDL_ALPHA_OPAQUE)!=0){
                        printf("changing drawing color failed \n");
                    }

                    if(SDL_RenderFillRect(r,&rct)!=0){
                        printf("drawing a filled rectangle failed \n");
                    }
                }

            }
        }SDL_RenderPresent(r);


}

int main(int argcc, char ** argv){

    SDL_Window * window=NULL;
    SDL_Renderer * renderer=NULL;

    t_GRAPH* graph;
    graph = create_graph(25,25);

    if(SDL_Init(SDL_INIT_EVERYTHING)!=0){
        SDL_exit_with_error("Initialization erreur");
    }

    window=SDL_CreateWindow("a_star similator",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,0);

    if(window==NULL){
        SDL_exit_with_error("Creating windows failed");
    }

    renderer=SDL_CreateRenderer(window ,-1, SDL_RENDERER_SOFTWARE);

    if(renderer==NULL){
        SDL_exit_with_error("Creating renderer failed");
    }
    if(SDL_SetRenderDrawColor(renderer,112,168,237,SDL_ALPHA_OPAQUE) !=0){
        SDL_exit_with_error("changing drawing color failed");
    }
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"INSTRUCTION","Clic gauche -> choisir le depart\nClic droite -> choisir la destination\nLEFT SHIFT + Glisser la Souris -> placer les obsctacles\nLEFT CTRL -> Lancer l'algorithme A*\nClic 'r' -> Restart",window);
    sdl_draw_graph(graph,renderer,-1,-1,-1,-1);


    SDL_bool program_launched=SDL_TRUE;
    SDL_bool start_selected=SDL_FALSE;
    SDL_bool end_selected=SDL_FALSE;
    int sx=-1,sy=-1,ex=-1,ey=-1;
    while(program_launched){
        SDL_Event event;

        while(SDL_PollEvent(&event)){

            switch(event.type){

                case SDL_KEYDOWN:
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_LCTRL:
                       {
                            if(sx==-1 || sy==-1 || ex==-1 || ey==-1)
                                continue;

                            t_NODE *path= a_star(graph,sx,sy,ex,ey);
                            sdl_draw_graph(graph,renderer,sx,sy,ex,ey);
                            path=reconstruct_path(path,&(graph->matrice[ex][ey]));
                            path=path->parent;
                            while(path!=&(graph->matrice[ex][ey])){
                                SDL_Rect rct = *(sdl_prepare_rectangle(renderer,path->x*(WINDOW_WIDTH/graph->cols),path->y*(WINDOW_HEIGHT/graph->rows),(WINDOW_HEIGHT/graph->rows),(WINDOW_WIDTH/graph->cols)));
                                if(SDL_SetRenderDrawColor(renderer,255,0,7,SDL_ALPHA_OPAQUE)!=0){
                                    printf("changing drawing color failed \n");
                                }

                                if(SDL_RenderFillRect(renderer,&rct)!=0){
                                    printf("drawing a filled rectangle failed \n");
                                }
                                path=path->parent;
                                SDL_Delay(100);
                                SDL_RenderPresent(renderer);
                            }
                            continue;
                       }
                       case SDLK_LSHIFT:
                       {
                            int xx,yy;
                            SDL_GetMouseState(&xx,&yy);
                            xx=xx / (WINDOW_WIDTH/graph->cols);
                            yy=yy / (WINDOW_HEIGHT/graph->rows);
                            graph->matrice[xx][yy].can_go=0;
                            sdl_draw_graph(graph,renderer,sx,sy,ex,ey);
                            continue;
                       }
                       case SDLK_r:
                       {
                            printf("restart !!\n");
                            free(graph);
                            graph=create_graph(25,25);
                            start_selected=SDL_FALSE;
                            end_selected=SDL_FALSE;
                            ex=-1;
                            ey=-1;
                            sx=-1;
                            sy=-1;
                            sdl_draw_graph(graph,renderer,-1,-1,-1,-1);
                            continue;
                       }

                    default:
                        continue;
                    }


                }
                case SDL_MOUSEBUTTONDOWN:
                   {
                    if(event.button.button==SDL_BUTTON_LEFT){

                        sx=event.motion.x / (WINDOW_WIDTH/graph->cols);
                        sy=event.motion.y / (WINDOW_HEIGHT/graph->rows);
                        sdl_draw_graph(graph,renderer,sx,sy,ex,ey);
                        SDL_RenderPresent(renderer);

                        printf("%d %d\n",sx,sy);

                        start_selected=SDL_TRUE;
                        continue;
                    }
                    if(event.button.button==SDL_BUTTON_RIGHT){


                        ex=event.motion.x / (WINDOW_WIDTH/graph->cols);
                        ey=event.motion.y / (WINDOW_HEIGHT/graph->rows);
                        sdl_draw_graph(graph,renderer,sx,sy,ex,ey);
                        SDL_RenderPresent(renderer);

                        printf("%d %d\n",ex,ey);

                        end_selected=SDL_TRUE;
                        continue;
                    }

                }

                case SDL_QUIT:
                    program_launched=SDL_FALSE;
                    break;
                default:
                break;
            }
        }
    }




    /*---------------------------------------------------------------------------------------------------*/

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
//gcc a_star/main.c -o bin/a -I include -L lib -lmingw32 -lSDL2main -lSDL2
//gcc a_star/test.c -o bin/a -I include -L lib -lmingw32 -lSDL2main -lSDL2
