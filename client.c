#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>


struct msgbuf
    {
        long mtype;
        char name[16];
        int id;
        int flag;
        key_t key;
        int id_kolejki;
        char msg[100];
        int option;
        int subskrypcja;
        int typ_wiadomosci;
        int liczba_wiadomosci;
        int priorytet;
        int odbior; 
        int liczba;
        int end;

    }message,rcvdmsg,reset,rcvdmsg2;

struct wiadomosci
{
        char wiadomosc[100];
        int priorytet;

};


void send_message(struct msgbuf mes, int id){
    if(msgsnd(id,&mes,sizeof(mes)-sizeof(long),0)==-1){
                
                perror("Client:msgsend");
            }
}

int main(int argc, char *argv[])
{

        struct wiadomosci odebrane_wiadomosci[16];
        int licz_proby=0;
        for(int i=0;i<16;i++){
                odebrane_wiadomosci[i].priorytet=0;

        }
        pid_t mypid;
        mypid = getpid();
        key_t key=1111123;
        key_t keyy=mypid;
        int opt;
        int qid = msgget(keyy, 0644 | IPC_CREAT);
        int queue_id = msgget(key, 0644 | IPC_CREAT);
        int childpid;

        if((childpid=fork())==0){

                    while(1){

                        if(msgrcv(qid,&rcvdmsg2,sizeof(rcvdmsg2)-sizeof(long),7,IPC_CREAT)==-1){

                        }

                        if(rcvdmsg2.flag==0){
                            printf("%s \n",rcvdmsg2.msg);
                        }
                        if(rcvdmsg2.flag==1){
                                printf("%s \n",rcvdmsg2.msg);
                                printf("%d \n",rcvdmsg2.typ_wiadomosci);
                        }


                     }
                }
        else{
        for(;;){

                message = reset;
                printf("\nMENU\n");
                printf(" 1 - Logowanie\n 2 - Rejestracja odbiorcy\n 3 - Rejestracja typu wiadomosci\n 4 - Rozgloszenie nowej wiadomosci\n 5 - Odbior wiadomosci\n 6 - Zablokowanie mo??liwo??ci odbioru wiadomo??ci od danego u??ytkownika\n 0 - Wylogowanie\n");
                while(scanf("%d", &opt)!=1){
                        printf("Prosze wpisac wartosc numeryczna z zakresu 0-5\n");

                        scanf("%*s");
                }

                message.option=opt;
                message.mtype = 1;
                message.id = mypid;
                message.key=keyy;
                message.id_kolejki = qid;

                if(message.option==0)
                {

                        kill(childpid,9);
                        msgctl(qid,IPC_RMID,0);
                        msgsnd(queue_id,&message,sizeof(message)-sizeof(long),0);

                        exit(0);

                }
                else if(message.option==1){

                        printf("Type username \n");
                        char username[16];
                label2:
                        scanf("%s", username);
                        int i=0;
                        while(username[i]){
                                if(!isalpha(username[i])){
                                        printf("Podaj nazw?? u??ytkownika s??ownie \n");
                                        goto label2;
                                }
                                i++;
                        }

            
                        strcpy(message.name, username);
                        send_message(message,queue_id);
                        if(msgrcv(qid,&rcvdmsg,sizeof(rcvdmsg)-sizeof(long),10,0)==-1){
                                 perror("Client:msgrcv");

                        }
                        if(rcvdmsg.flag==1){
                                licz_proby++;
                        }
                        if(rcvdmsg.flag==2){
                                printf("%s\n",rcvdmsg.msg);
                                kill(childpid,9);
                                msgctl(qid,IPC_RMID,0);
                        
                                exit(0);
                        }
                        if(licz_proby==3){
                                printf("Przekroczono limit pr??b logowania\n");
                                kill(childpid,9);
                                msgctl(qid,IPC_RMID,0);
                              
                                exit(0);
                        }

                        printf("%s\n",rcvdmsg.msg);

                }
                else if(message.option==2){

                        printf("Podaj nazw?? u??ytkownika \n");
                        char username[16];
                label3:
                        scanf("%s", username);
                        int i=0;
                        while(username[i]){
                                if(!isalpha(username[i])){
                                        printf("Podaj nazw?? u??ytkownika s??ownie \n");
                                        goto label3;
                                }
                                i++;
                        }

                        strcpy(message.name, username);
                        printf("Podaj typ wiadomosci, na jaki chcesz si?? zarejestrowa?? \n");
                        int typ;

                        while(scanf("%d", &typ)!=1){
                        printf("Prosze wpisac wartosc numeryczna\n");

                                scanf("%*s");
                        }
                        message.typ_wiadomosci = typ;

                        printf("Podaj 1, jesli subskrypcja ma byc trwala, lub 2 jesli ma byc przejsciowa\n");
                        int sub;

                label:
                        while(scanf("%d", &sub)!=1){
                                printf("Prosze wpisac wartosc numeryczna\n");

                                scanf("%*s");
                        }

                        if(sub<1 || sub>2){
                                printf("Prosz?? poda?? cyfr?? 1 - subskrypcja trwala, lub 2 - subskrypcja przejsciowa\n");
                                goto label;
                        }
                        message.subskrypcja = sub;
                        if(message.subskrypcja==2){
                              printf("Podaj, ile razy chcesz otrzymac wiadomosc danego typu\n");
                              int cnt;

                              while(scanf("%d", &cnt)!=1){
                                        printf("Prosze wpisac wartosc numeryczna\n");

                                        scanf("%*s");
                                }
                                    
                              message.liczba_wiadomosci = cnt;
                        }
                         send_message(message,queue_id);

 
                        rcvdmsg = reset;

                        if(msgrcv(qid,&rcvdmsg,sizeof(rcvdmsg)-sizeof(long),4,0)==-1){
                                 perror("Client:msgrcv");
                        }


                        printf("%s \n",rcvdmsg.msg);


                }

                else if(message.option==3){

                        printf("Type username \n");
                        char user[16];
                label4:
                        scanf("%s", user);
                        int i=0;
                        while(user[i]){
                                if(!isalpha(user[i])){
                                        printf("Podaj nazw?? u??ytkownika s??ownie \n");
                                        goto label4;
                                }
                                i++;
                        }
                   
                        strcpy(message.name, user);
                        printf("Podaj typ wiadomosci, kt??ry chcesz rozg??asza?? \n");
                        int typ;
                        while(scanf("%d", &typ)!=1){
                        printf("Prosze wpisac wartosc numeryczna\n");

                                scanf("%*s");
                        }

                        message.typ_wiadomosci = typ;

                        send_message(message,queue_id);

                        rcvdmsg = reset;

                        if(msgrcv(qid,&rcvdmsg,sizeof(rcvdmsg)-sizeof(long),3,0)==-1){
                                perror("Client:msgrcv");

                                }

                        printf("%s \n", rcvdmsg.msg);
                }
                else if(message.option==4){
            
                        printf("Podaj typ wiadomo??ci, kt??ry chcesz rozg??asza??\n");
                        int typ;
                        while(scanf("%d", &typ)!=1){
                        printf("Prosze wpisac wartosc numeryczna\n");

                                scanf("%*s");
                        }

                    
                        message.typ_wiadomosci = typ;
                    
                        printf("Podaj priorytet wiadomo??ci od 0 do 5\n");
                        int priorytet;
                label5:
                        while(scanf("%d", &priorytet)!=1){
                                printf("Prosze wpisac wartosc numeryczna\n");

                                scanf("%*s");
                        }

                        if(priorytet<0 || priorytet>5){
                                printf("Prosz?? poda?? cyfr?? z zakresu 0-5\n");
                                goto label5;
                        }

                        message.priorytet = priorytet;
                        printf("Podaj tre????  wiadomo??ci\n");
                        char tresc[100];
                        label6:
                        scanf(" %[^\n]s",tresc);
                        int i=0;
                        while(tresc[i]){
                                if(isdigit(tresc[i])){
                                        printf("Wiadomo???? nie powinna zawiera?? znak??w numerycznych. Podaj wiadomo???? ponownie\n");
                                        goto label6;
                                }
                                i++;
                        }

                        strcpy(message.msg, tresc);
                        send_message(message,queue_id);
                        rcvdmsg = reset;
                        if(msgrcv(qid,&rcvdmsg,sizeof(rcvdmsg)-sizeof(long),5,0)==-1){
                                perror("Client:msgrcv");

                                }
                        printf("%s \n", rcvdmsg.msg);

                }
                else if(message.option==5){
         
                        printf("Je??li chcesz odbierac podany typ wiadomo??ci w sposob synchroniczny wybierz 1, je??li w spos??b asynchroniczny wybierz 2 \n");
                        int sposob;
                label7:
                        while(scanf("%d", &sposob)!=1){
                                printf("Prosze wpisac wartosc numeryczna\n");

                                scanf("%*s");
                        }
                        if(sposob<1 || sposob>2){
                                printf("Prosz?? poda?? cyfr?? 1 - odbior synchroniczny, lub 2 - odbior asynchroniczny\n");
                                goto label7;
                        }

                        message.odbior = sposob;
                        int typ;
                        
        
                        printf("Jaki typ wiadomo??ci chcesz odebrac z tych, na kt??re si?? zarejestrowa??e???\n");
                        while(scanf("%d", &typ)!=1){
                        printf("Prosze wpisac wartosc numeryczna\n");

                                scanf("%*s");
                        }

                        message.typ_wiadomosci = typ;

                        send_message(message,queue_id);

                        if(sposob==1){

                        int cnt=0;
                        rcvdmsg = reset;
                        while(rcvdmsg.end==0){ 
                                rcvdmsg=reset;
                                msgrcv(qid,&rcvdmsg,sizeof(rcvdmsg)-sizeof(long),8,0);


                                if(rcvdmsg.priorytet!=0){
                                         strcpy(odebrane_wiadomosci[cnt].wiadomosc,rcvdmsg.msg);
                                         odebrane_wiadomosci[cnt].priorytet = rcvdmsg.priorytet;
                                         cnt++;
                                }
                                if(rcvdmsg.flag==1){
                                        printf("%s \n",rcvdmsg.msg);
                                }
                        }


                        struct wiadomosci temp;

                        for (int i = 1; i<cnt;i++)
                                for (int j=0;j<cnt-i;j++) {
                                        if (odebrane_wiadomosci[j].priorytet<odebrane_wiadomosci[j+1].priorytet) {
                                                temp = odebrane_wiadomosci[j];
                                                odebrane_wiadomosci[j] = odebrane_wiadomosci[j+1];
                                                odebrane_wiadomosci[j+1] = temp;
                                                }
                                }
                        for(int i=0;i<cnt;i++){
                                printf("%s \n", odebrane_wiadomosci[i].wiadomosc);
                        }
                }

               
            }
             else if(message.option==6){

                   printf("Type username \n");
                        char user[16];
                label8:
                        scanf("%s", user);
                        int g=0;
                        while(user[g]){
                                if(!isalpha(user[g])){
                                        printf("Podaj nazw?? u??ytkownika s??ownie \n");
                                        goto label8;
                                }
                                g++;
                        }
                   message.flag=1;
                   strcpy(message.name, user);
      

                   send_message(message,queue_id);

                   rcvdmsg = reset;
                        while(rcvdmsg.end==0){
                                rcvdmsg=reset;
                                msgrcv(qid,&rcvdmsg,sizeof(rcvdmsg)-sizeof(long),11,0);

                                if(rcvdmsg.flag==1){
                                        printf("\n%s \n",rcvdmsg.msg);
                                }

                                if(rcvdmsg.flag==0){
                                        printf("%s  ",rcvdmsg.name);
                                   
                                }
                        }
                        printf("\nPodaj nazwe u??ytkownika, od kt??rego nie chcesz otrzymywa?? wiadomo??ci\n");
                         char user2[16];
                label9:
                        scanf("%s", user2);
                        int k=0;
                        while(user2[k]){
                            if(!isalpha(user2[k])){
                                        printf("Podaj nazw?? u??ytkownika s??ownie \n");
                                        goto label9;
                                        }
                                k++;
                        }
                        message.flag=2;
                        strcpy(message.name, user2);
                        send_message(message,queue_id);

                }

        }
    }

return 0;
}