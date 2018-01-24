//
//  main.c
//  beer
//
//  Created by joye on 2018/1/17.
//  Copyright © 2018年 joye. All rights reserved.
//

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#import "sqlite3.h"

#define arrayHigh 30  //总数组高度
#define arrayWide 5   //总数组宽度
#define runPoint   25 //计算区域开始行
#define runHigh   5 //计算区域高度

#define gemCount 5  //宝石数量
#define treeRemove 3 //三消.....三次以上就消除

typedef struct
{
    int x;
    int y;
}Point;

typedef struct
{
    int multiple; //倍数
    int dropCount; //下落次数
    int allCount;  //总消除次数
    int green3;    //颜色
    int green4;
    int green5;
    int bule3;
    int bule4;
    int bule5;
    int yellow3;
    int yellow4;
    int yellow5;
    int red3;
    int red4;
    int red5;
    int purple3;
    int purple4;
    int purple5;
    int line5;
}Tdata;


void singleStat(Tdata *data,int colour,int combo){
    //消除总次数加一
    data->allCount += 1;
    int nowMultiple = 0; //当前倍率
    switch (colour) {
        case 1:
            //绿
            if (combo == 3){
                nowMultiple = 1;
                data->green3 += 1;
            }else if (combo == 4){
                nowMultiple = 5;
                data->green4 += 1;
            }else if (combo >= 5){
                nowMultiple = 10;
                data->green5 += 1;
            }
            break;
        case 2:
            //蓝
            if (combo == 3){
                nowMultiple = 2;
                data->bule3 += 1;
            }else if (combo == 4){
                nowMultiple = 6;
                data->bule4 += 1;
            }else if (combo >= 5){
                nowMultiple =  12;
                data->bule5 += 1;
            }
            break;
        case 3:
            //黄
            if (combo == 3){
                nowMultiple =  3;
                data->yellow3 += 1;
            }else if (combo == 4){
                nowMultiple = 7;
                data->yellow4 += 1;
            }else if (combo >= 5){
                nowMultiple =  14;
                data->yellow5 += 1;
            }
            break;
        case 4:
            //红
            if (combo == 3){
                nowMultiple = 4;
                data->red3 += 1;
            }else if (combo == 4){
                nowMultiple = 8;
                data->red4 += 1;
            }else if (combo >= 5){
                nowMultiple =  16;
                data->red5 += 1;
            }
            break;
        case 5:
            //紫
            if (combo == 3){
                nowMultiple =  5;
                data->purple3 += 1;
            }else if (combo == 4){
                nowMultiple =  10;
                data->purple4 += 1;
            }else if (combo >= 5){
                nowMultiple =  20;
                data->purple5 += 1;
            }
            break;
        default:
            break;
    }
    //总倍率加当次倍率
    if (combo >= 5 ){
        data->line5 += 1 ;
    }
    
    data->multiple += nowMultiple;
}

//创建随机数组
void createRandomArray(int (*containers)[arrayHigh]){
    srand(clock()*rand()%(10000));//以时间为种子，每次生成不同的数
    for(int i=0;i<arrayWide;i++)
    {
        for (int k= 0;k < arrayHigh;k ++){
           containers[i][k]=1+rand()%(gemCount);//rand()生成数的范围在[a,b)写成：a[i]=a+rand()%(b-a);
        }
    }
}
//打印随机数组
void printArray(int (*containers)[arrayHigh]){
    for (int k= 0;k < arrayHigh;k ++){
        for(int i=0;i<arrayWide;i++)
        {
             printf("%d ", containers[i][k]);
        }
        printf("\n");
    }
}
//标记消除模块
void alter(int (*containers)[arrayHigh],Point *treeRemoveArry,int *treeRemoveArryCount){
    for (int i =0;i < *treeRemoveArryCount;i++){
        containers[treeRemoveArry[i].x][treeRemoveArry[i].y] = 0;
    }
}

//寻找消除模块
void FindRemoveNode(int (*containers)[arrayHigh],Point *treeRemoveArry ,int *treeRemoveArryCount,Tdata *data){
    //横向遍历结算区域
    for (int k= runPoint;k < runPoint+runHigh;k ++){
        int repetition = 0;
        int combo = 1;
        
        for(int i=0;i < arrayWide;i++)
        {
            if (containers[i][k] == repetition ){
                combo += 1;
            }
            //到结尾的判断
            if (i == arrayWide-1)
            {
                if(combo >= treeRemove ){
                    if (containers[i][k] != repetition){
                       //printf("发生了横向三消起始坐标%d %d 结束坐标 %d %d 三消颜色为%d \n",i-combo,k,i-1,k,repetition);
                        for (int j = 0; j < combo;j++) {
                            //printf("三消图标 %d  %d \n",i - j -1  ,k);
                            treeRemoveArry[*treeRemoveArryCount].x =i - j -1;
                            treeRemoveArry[*treeRemoveArryCount].y =k;
                            *treeRemoveArryCount += 1;
                        }
                         singleStat(data,repetition,combo);
                    }else{
                        //printf("发生了横向三消起始坐标%d %d 结束坐标 %d %d 三消颜色为%d \n",i-combo+1,k,i,k,repetition);
                        for (int j = 0; j < combo;j++) {
                            //printf("三消图 %d  %d \n",i - j  ,k);
                            treeRemoveArry[*treeRemoveArryCount].x =i - j ;
                            treeRemoveArry[*treeRemoveArryCount].y =k;
                            *treeRemoveArryCount += 1;
                        }
                        singleStat(data,repetition,combo);
                    }
                }
                continue;
            }
            
           //判断三消
            if (containers[i][k] != repetition){
                if(combo >= treeRemove ){
                   // printf("发生了横向三消起始坐标%d %d 结束坐标 %d %d 三消颜色为%d \n",i-combo,k,i,k,repetition);
                    //三消node记入数组
                    for (int j = 0; j < combo;j++) {
                        //printf("三消图形 %d  %d \n",i - j -1  ,k);
                        treeRemoveArry[*treeRemoveArryCount].x =i - j -1;
                        treeRemoveArry[*treeRemoveArryCount].y =k;
                        *treeRemoveArryCount += 1;
                    }
                    singleStat(data,repetition,combo);
                }else{
                         repetition = containers[i][k];
                }
                 combo = 1;
            }
            //初始化重复值
            if (i == 0){
                repetition = containers[i][k];
            }
        }
     }
    //纵向遍历计算区域
    for(int i=0;i<arrayWide;i++){
        int repetition = 0;
        int combo = 1;
       for  (int x = arrayHigh-1; x > arrayHigh - runHigh -1;x--){
            if (containers[i][x] == repetition ){
                combo += 1;
            }
           
           //到结尾的判断
           if (x == arrayHigh - runHigh )
           {
               if(combo >= treeRemove ){
                   if (containers[i][x] != repetition){
                        //printf("发生了纵向三消起始坐标%d %d 结束坐标 %d %d 三消颜色为%d \n",i,x+combo,i,x+1,repetition);
                       //三消node记入数组
                       for (int j = 0; j < combo;j++) {
                           //printf("三消图形 %d  %d \n",i,x + j + 1);
                           treeRemoveArry[*treeRemoveArryCount].x =i;
                           treeRemoveArry[*treeRemoveArryCount].y =x + j + 1 ;
                           *treeRemoveArryCount += 1;
                       }
                       singleStat(data,repetition,combo);
                   }else{
                       //printf("发生了纵向三消起始坐 标%d %d 结束坐标 %d %d 三消颜色为%d \n",i,x+combo-1,i,x,repetition);
                       //三消node记入数组
                       for (int j = 0; j < combo;j++) {
                           //printf("三消图形 %d  %d \n",i,x + j);
                           treeRemoveArry[*treeRemoveArryCount].x =i;
                           treeRemoveArry[*treeRemoveArryCount].y =x + j ;
                           *treeRemoveArryCount += 1;
                       }
                       singleStat(data,repetition,combo);
                   }
               }
               continue;
           }
           //判断三消
           if (containers[i][x] != repetition){
               if(combo >= treeRemove ){
                  // printf("发生了纵向三消起始坐标%d %d 结束坐标 %d %d 三消颜色为%d \n",i,x+combo,i,x+1,repetition);
                   //三消node记入数组
                   for (int j = 0; j < combo;j++) {
                       //printf("三消图形 %d  %d \n",i,x + j +1);
                       treeRemoveArry[*treeRemoveArryCount].x =i;
                       treeRemoveArry[*treeRemoveArryCount].y =x + j +1;
                       *treeRemoveArryCount += 1;
                   }
                   singleStat(data,repetition,combo);
               }else{
                   repetition = containers[i][x];
               }
               combo = 1;
           }
           //初始化重复值
           if (x ==  arrayHigh-1){
               repetition = containers[i][x];
           }
            //printf("%d ", containers[i][runHigh+k]);
        }
    }
}



void allStatistics(Tdata *data,int dropCount){
    //下落总次数
    data->dropCount += dropCount;
}


//单屏下落
void drop(int (*containers)[arrayHigh],Tdata *data){
    int isDrop = 0;
    
    for(int i = 0;i < arrayWide;i++){
        //这里每一行向上遍历5次,有优化空间
        for (int k= 0;k < runHigh;k ++){
        for  (int x = arrayHigh; x > arrayHigh - runHigh ;x--){
            if (containers[i][x-1] == 0){
                //循环高度的次数
                isDrop += 1;
                for (int j=0;j < x;j++){
                    //printArray(containers);
                    containers[i][x-j-1] = containers[i][x-j-2];
                        //如果当前高度是顶部就置0
                    if (x-j-1 == 0){
                        containers[i][0] = 0;
                            //printf("\n");
                    }
                }
            }
         }
       }
    }
    
    if (isDrop > 0){
    //有下落就加1
        allStatistics(data,1);
    }
}

sqlite3 * pDB = NULL;

void run (){
    //生成一个随机数
    int containers[arrayWide][arrayHigh];
    createRandomArray(containers);
    Tdata data;
    memset(&data, 0, sizeof(data));
    //    int containers[arrayWide][arrayHigh] ={
    //        {5,9,4,2,6,4,3,2,1,5,2,3,2,1,5,5,9,4,2,6,4,3,2,1,5,2,2,2,2,2},
    //        {2,1,3,1,5,1,3,2,1,5,1,3,2,1,5,2,1,3,1,5,1,3,2,1,5,1,2,2,2,5},
    //        {1,1,5,4,4,4,4,3,2,2,1,3,3,1,5,1,1,5,4,4,4,4,3,2,2,1,3,3,1,5},
    //        {1,2,5,1,3,4,3,2,1,5,1,3,2,2,5,1,2,5,1,3,4,3,2,1,5,1,3,2,2,5},
    //        {4,4,2,4,2,4,3,2,1,5,2,2,2,2,5,4,4,2,4,2,4,3,2,1,5,2,2,2,2,5}
    //    };

//    int test[arrayWide*arrayHigh] = {4,5,1,3,2,1,1,1,1,4,4,1,3,5,4,3,1,4,2,3,2,3,5,5,3,1,3,3,4,4,1,2,5,3,1,4,5,4,5,5,1,3,4,1,3,4,1,4,1,1,1,4,3,1,5,1,5,3,5,5,2,5,1,4,1,5,4,2,1,4,5,4,2,2,4,2,3,3,1,5,1,3,3,1,5,1,5,4,3,2,1,1,2,2,1,3,3,2,1,3,2,4,1,5,3,5,5,5,4,5,4,5,5,1,4,2,4,3,4,3,1,2,5,4,1,5,1,1,3,5,1,1,3,5,5,3,5,5,3,3,2,2,1,1,4,2,3,4,1,1};
//    for (int i=0,j=0 ;i <= arrayWide*arrayHigh;i = i + arrayHigh,j++){
//        for (int k = i ; k <= i + arrayHigh -1 ;k++){
//            containers[j][i + arrayHigh - 1 - k] = test[k];
//                 //printf("---%d=%d=%d \n",k,i + arrayHigh - 1 - k,j);
//        }
//
//    }

    //打印数组
    char output[1000]="";
    for (int i = 0 ;i < arrayWide;i++ ){
        char buff[100];
        if (i == arrayWide -1){
            sprintf( buff ,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",containers[i][29],containers[i][28],containers[i][27],containers[i][26],containers[i][25],containers[i][24],containers[i][23],containers[i][22],containers[i][21],containers[i][20],containers[i][19],containers[i][18],containers[i][17],containers[i][16],containers[i][15],containers[i][14],containers[i][13],containers[i][12],containers[i][11],containers[i][10],containers[i][9],containers[i][8],containers[i][7],containers[i][6],containers[i][5],containers[i][4],containers[i][3],containers[i][2],containers[i][1],containers[i][0]);
        }else{
            sprintf( buff ,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,",containers[i][29],containers[i][28],containers[i][27],containers[i][26],containers[i][25],containers[i][24],containers[i][23],containers[i][22],containers[i][21],containers[i][20],containers[i][19],containers[i][18],containers[i][17],containers[i][16],containers[i][15],containers[i][14],containers[i][13],containers[i][12],containers[i][11],containers[i][10],containers[i][9],containers[i][8],containers[i][7],containers[i][6],containers[i][5],containers[i][4],containers[i][3],containers[i][2],containers[i][1],containers[i][0]);
        }
        strcat(output,buff);
    }
    
    
    for (int i = 0 ;i < arrayHigh / runHigh;i++ ){
        //生成移除数组
        Point treeRemoveArry[arrayHigh*arrayWide];
        int treeRemoveArryCount = 0;
        //printArray(containers);
        FindRemoveNode(containers,treeRemoveArry,&treeRemoveArryCount,&data);
        alter(containers,treeRemoveArry,&treeRemoveArryCount);
        if (treeRemoveArryCount == 0){
            //当前屏无法消除情况下跳出
            break;
        }
        //printf("标注后\n");
        //printArray(containers);
        drop(containers,&data);
        //printf("下落后\n");
       // printArray(containers);
        //printf("\n");
    }
    char sql[1000];
    sprintf(sql,"INSERT INTO simulate (multiple,dropCount,allCount,green3,green4,green5,bule3,bule4,bule5,yellow3,yellow4,yellow5,red3,red4,red5,purple3,purple4,purple5,line5,data)VALUES(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'{\"array\":[%s]}')",data.multiple, data.dropCount, data.allCount,data.green3,data.green4,data.green5,data.bule3,data.bule4,data.bule5, data.yellow3,data.yellow4,data.yellow5,data.red3,data.red4,data.red5,data.purple3,data.purple4,data.purple5,data.line5,output);
    char* cErrMsg;
    sqlite3_exec(pDB , sql ,0 ,0, &cErrMsg);
}

int main(int argc, const char * argv[]) {
    
    int nRes = sqlite3_open("test.db", &pDB);
    if (nRes != SQLITE_OK)
    {
        printf("hello world");
    }

    for(int i=0;i<1000000;i++){
         printf("目前循环第 %d 次 \n",i+1);
      run();
    }

    return 0;
}
