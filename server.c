#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX 16
#define MAX2 50


int counter=0;

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
    }message,reset,msg_r,mes,mes2;

struct baza_uzytkownikow
{
        char name[16];
        int id;
        key_t key;
        int id_kolejki;
        
};


struct baza_nadawcow
{
    int typ_wiadomosci;
        int nadawca;

};

struct zapisani_odbiorcy
{
        int typ_wiadomosci;
        int id;
        int liczba_wiadomosci;

};

struct tresci_wiadomosci
{
        char tresc_wiadomosci[100];
        int typ_wiadomosci;
        int priorytet;
        int nr_wiadomosci;
};

struct info_o_kliencie
{
        int typ_wiadomosci;
        int id;
        int id_kolejki;
        int subskrypcja;
        int czy_czeka;
        int wiadomosci_wyslane[30];
        int typ_subskrypcji;
        int liczba_wiadomosci;

};

struct blokowanie
{
        int id_nadawcy;
        int id_odbiorcy;
};

struct baza_uzytkownikow users[MAX];
struct baza_nadawcow baza_nadawcow[MAX];
struct tresci_wiadomosci tablica_wiadomosci[30];
struct info_o_kliencie klienci_info[30];
struct zapisani_odbiorcy odbiorcy[MAX2];
struct blokowanie dont_receive[MAX];

void send_message(struct msgbuf mes, int id){
    if(msgsnd(id,&mes,sizeof(mes)-sizeof(long),0)==-1){

                perror("Client:msgsend");
            }
}

void wysylanie_asynchroniczne(int typ, struct msgbuf message,char wiad[100]){

    int flag=0,id_kolejki,id_nadawcy,dont_send=0;
 

    for(int i=0;i<30;i++){//server automatically sends a message to clients who wish to receive it asynchronously

    if(klienci_info[i].typ_wiadomosci == typ && klienci_info[i].subskrypcja ==2){

            for(int i=0;i<16;i++){
                   if(baza_nadawcow[i].typ_wiadomosci==typ){

                           id_nadawcy =baza_nadawcow[i].nadawca;
                           break;
                   }
            }

            for(int g=0;g<MAX;g++){
                  if(dont_receive[g].id_nadawcy==id_nadawcy && dont_receive[g].id_odbiorcy==klienci_info[i].id){
                           dont_send=1;
                           goto lab2;

                 }
           }

            flag=0;
            if(klienci_info[i].typ_subskrypcji==2){
                    if(klienci_info[i].liczba_wiadomosci==0){
                        klienci_info[i].subskrypcja=0;
                        klienci_info[i].typ_wiadomosci=0;
                        klienci_info[i].typ_subskrypcji=0;
                        klienci_info[i].id_kolejki=0;
                        for(int k=0;k<30;k++){
                                klienci_info[i].wiadomosci_wyslane[k]=0;
                        }
                        flag=1;
                    }
                    else{
                        klienci_info[i].liczba_wiadomosci--;
                    }

            }

            if(flag==0){

    lab2:
            message = reset;
            id_kolejki = klienci_info[i].id_kolejki;
            if(dont_send==0){
            message.flag=0;
            strcpy(message.msg,wiad);
            }
            message.mtype=7;
            message.id = id_nadawcy;
            if(dont_send==1){
                message.flag=2;
            }
            if(msgsnd(id_kolejki,&message,sizeof(message)-sizeof(long),0)==-1){
                                perror("Server:msgsnd");

                }
            }

    }
    }
}

void sprawdzanie(int typ, struct msgbuf message, int id,int client_id){

    int flag=0,flag2=0,flag3=0,numer,priorytet=0,check=0,found=0,id_nadawcy;

    for(int k=0;k<30;k++){

          if(klienci_info[k].id == client_id && klienci_info[k].typ_wiadomosci == typ && klienci_info[k].subskrypcja==1){
                   if(klienci_info[k].typ_subskrypcji==2){//I am checking to see if customers with the transition subscription can still receive messages
                            check=1;
                     
                            if(klienci_info[k].liczba_wiadomosci==0){
                                         flag2=1;
                                         break;
                            }

                  }
          }
    }
    
        if(flag2==0){
                for(int j=0;j<30;j++){//sending all messages of this type that have not yet been sent synchronously to the client
                    message = reset;
                        flag=0;
                        if(tablica_wiadomosci[j].typ_wiadomosci==typ){

                            for(int i=0;i<16;i++){
                                        if(baza_nadawcow[i].typ_wiadomosci==typ){

                                                id_nadawcy =baza_nadawcow[i].nadawca;
                                                break;
                                        }
                                }

                                for(int i=0;i<MAX;i++){
                                        if(dont_receive[i].id_nadawcy==id_nadawcy && dont_receive[i].id_odbiorcy==client_id){

                                                found=1;
                                                flag=1;
                                               
                                                break;

                                        }
                                }

                                numer = tablica_wiadomosci[j].nr_wiadomosci;
                             
                                priorytet = tablica_wiadomosci[j].priorytet;
                                strcpy(message.msg,tablica_wiadomosci[j].tresc_wiadomosci);



                                for(int k=0;k<30;k++){

                                        if(klienci_info[k].id == client_id && klienci_info[k].typ_wiadomosci == typ && klienci_info[k].subskrypcja==1){

                                                if(klienci_info[k].liczba_wiadomosci==0 && check==1){
                                                        flag3=1;
                                                }


                                            if(flag3==0){
                                                for(int w=0;w<30;w++){
                                                        if(klienci_info[k].wiadomosci_wyslane[w]==numer){
                                                                flag=1;
                                                                break;
                                                        }
                                                        if(klienci_info[k].wiadomosci_wyslane[w]==0 && flag==0){
                                                                found=1;
                                                                if(check){
                                                                        klienci_info[k].liczba_wiadomosci--;
                                                                        
                                                                }
                                                                klienci_info[k].wiadomosci_wyslane[w]=numer;
                                                                break;
                                                        }
                                                }
                                        }
                                }

                        }
                    if(flag==0 && flag2==0 &&flag3==0&& found==1){

        
                                        message.mtype=8;
                                        message.end=0;
                                        message.priorytet = priorytet;
                                        message.id = id_nadawcy;
                                        if(msgsnd(id,&message,sizeof(message)-sizeof(long),0)==-1){
                                                perror("Server:msgsnd");
                                        }

                                }
                    }
                }
                if(found==0){//there is no messageas with type the client wants yet, client is being blocked

                        for(int a=0;a<30;a++){
                                if(klienci_info[a].id == client_id && klienci_info[a].typ_wiadomosci == typ && klienci_info[a].subskrypcja==1){
                                        klienci_info[a].czy_czeka=1;
                                        break;
                                }
                        }
                }
        }

        if(found || flag2==1){//sending messages if the type was in the system or if messages with transitional subscription weren't allowed for this type anymore
                message = reset;
                message.mtype=8;
                message.end=1;
                if(msgsnd(id,&message,sizeof(message)-sizeof(long),0)==-1){
                        perror("Server:msgsnd");
                }
        }

}

int zalogowani_uzytkownicy=0;


int main(int argc, char *argv[])
{
    for(int i=0;i<MAX;i++){
                 baza_nadawcow[i].typ_wiadomosci=0;
                 baza_nadawcow[i].nadawca = 0;
                 users[i].id=0;
                 dont_receive[i].id_nadawcy=0;
                 dont_receive[i].id_odbiorcy=0;
                 klienci_info[i].typ_wiadomosci=0;
                 klienci_info[i].liczba_wiadomosci=0;
                 klienci_info[i].typ_subskrypcji=0;
                 klienci_info[i].czy_czeka=0;
                 for(int j=0;j<30;j++){
                        klienci_info[i].wiadomosci_wyslane[j]=0;
                 }
        }

        for(int i=0;i<30;i++){
                tablica_wiadomosci[i].typ_wiadomosci=0;

        }

        for(int i=0;i<MAX2;i++){

            odbiorcy[i].typ_wiadomosci=0;
            odbiorcy[i].id =0;
            odbiorcy[i].liczba_wiadomosci=0;
        }


        int queue_id = msgget(1111123, 0644 | IPC_CREAT);
 
        key_t keyy;
        for(;;){

                msg_r = reset;
                msgrcv(queue_id, &msg_r, sizeof(msg_r)-sizeof(long), 1, 0);
                keyy = msg_r.key;
                int qid = msgget(keyy,0644 | IPC_CREAT);

                if(msg_r.option==1){
                        message=reset;      
                        int done=0;                      
                        message.mtype= 10;
                        strcpy(message.name, msg_r.name);
                   
                        for(int i=0;i<MAX;i++){
                                if(counter==MAX){
                                        done=2;
                                        break;
                                }

                                if(strcmp(users[i].name,msg_r.name)==0 || users[i].id == msg_r.id){
                            
                                        break;
                                }

                                else if(strcmp(users[i].name,"")!=0){
                                        continue;

                                }
                                else if(users[i].id==0){
                                        users[i].id = msg_r.id;
                                        strcpy(users[i].name,msg_r.name);                                  
                                        users[i].id_kolejki = msg_r.id_kolejki;                                     
                                        done=1;
                                        break;
                                }
                          
                        }
                        if(done==1){

                                strcpy(message.msg, "Logowanie powiodło się");
                                zalogowani_uzytkownicy++;
                                counter++;
                        }
                        if(done==0){
                                message.flag=1;
                                strcpy(message.msg, "Podany użytkownik jest już zalogowany");
                        }
                        if(done==2){
                                message.flag=2;

                                strcpy(message.msg, "System osiągnął maksymalną liczbę uzytkowników");
                        }
                        send_message(message,qid);

            }
            else if(msg_r.option==0){

                        zalogowani_uzytkownicy--;

                        if(zalogowani_uzytkownicy==0){
                                msgctl(queue_id,IPC_RMID,0);
                                msgctl(qid,IPC_RMID,0);
                                exit(0);
                        }

                }
                else if(msg_r.option==2){

                        message = reset;
                        message.mtype=4;
                        bool control=true;

                        for(int i=0;i<MAX;i++){
                                 if(strcmp(users[i].name,msg_r.name)==0){
                                        control=false;
                                        break;
                                }
                        }
                        if(control){

                                strcpy(message.msg,"Uzytkownik nie jest zalogowany");
                        }
                        else if(!control){

                                int flg2=0;


                                for(int i=0;i<MAX;i++){
                                        if(baza_nadawcow[i].typ_wiadomosci==msg_r.typ_wiadomosci){
                                            flg2=1;
                                        }
                                }
                                if(flg2){

                                    for(int i=0;i<MAX2;i++){
                                        if(odbiorcy[i].typ_wiadomosci==0){
                                                strcpy(message.msg, "Odbiorca został pomyślnie zarejestrowany");
                                                odbiorcy[i].typ_wiadomosci = msg_r.typ_wiadomosci;
                                                odbiorcy[i].id = msg_r.id;
                                                if(msg_r.subskrypcja==2){
                                                    odbiorcy[i].liczba_wiadomosci= msg_r.liczba_wiadomosci;
                                                }
                                                else{
                                                    odbiorcy[i].liczba_wiadomosci= 0;

                                                }
                                                break;
                                            }

                                        }

                                }

                                else if(flg2==0){
                                        strcpy(message.msg, "Ten typ wiadomosci nie jest jeszcze zarejestrowany");
                                }
                        }

                        send_message(message,qid);
         
                }
                else if(msg_r.option==3){
                        message = reset;
                        message.mtype= 3;
                        bool control=true;
                        int ck=0;

                        for(int i=0;i<MAX;i++){
                                 if(strcmp(users[i].name,msg_r.name)==0){

                                        control=false;
                                        break;

                                }
                        }
                        if(control){

                                strcpy(message.msg, "Uzytkownik nie jest zalogowany");
                        }

                        else if(!control){
                            for(int i=0;i<MAX;i++){
                                        if(baza_nadawcow[i].typ_wiadomosci==msg_r.typ_wiadomosci){
                                                strcpy(message.msg, "Podany typ wiadomosci jest juz zarejestrowany. Proszę podać unikalny typ wiadomości.");
                                                ck=1;
                                        }
                                        else if(baza_nadawcow[i].typ_wiadomosci==0 && ck==0){
                                                baza_nadawcow[i].typ_wiadomosci = msg_r.typ_wiadomosci;
                                                baza_nadawcow[i].nadawca = msg_r.id;

                                                break;
                                        }

                                }
                                        
                                if(ck==0){
       
                                        message.flag=1;
                                        message.mtype=7;
                                        strcpy(message.msg, "W systemie pojawił się nowy typ wiadomości");
                                        message.typ_wiadomosci=msg_r.typ_wiadomosci;
                                        for(int j=0;j<30;j++){
                                                if(users[j].id!=0&&users[j].id!=msg_r.id){

                                                        msgsnd(users[j].id_kolejki,&message,sizeof(message)-sizeof(long),0);
                                                }
                                        }

                                        message = reset;
                                        message.mtype=3;
                                        strcpy(message.msg, "Podany typ wiadomosci został pomyślnie zarejestrowany");
                                        send_message(message,qid);

                                        }

                                }


                        if(ck==1 || control){
                                send_message(message,qid);
        
                        }

                }
                else if(msg_r.option==4){
                    int flg=0,last;
                        bool control=true;
                       
                        message = reset;

                        for(int i=0;i<MAX;i++){
                                 if(users[i].id==msg_r.id){
                                        control=false;
                                        break;

                                }
                        }
                        if(control){
               
                                strcpy(message.msg,"Uzytkownik nie jest zalogowany");
                        
                        }

                        for(int i=0;i<MAX;i++){

                                if(baza_nadawcow[i].typ_wiadomosci==msg_r.typ_wiadomosci && baza_nadawcow[i].nadawca==msg_r.id){

                                        flg=1;
                                        break;

                                }
                        }
                        if(flg==0){


                                strcpy(message.msg, "Uzytkownik nie rozgłasza danego tematu");
                        }


                        if(control || flg==0){

                                message.mtype=5;

                                send_message(message,qid);
                   
                        }


                        else if(!control && flg==1){

                                for(int i=0;i<30;i++){
                                        if(tablica_wiadomosci[i].typ_wiadomosci==0){
                                                 strcpy(tablica_wiadomosci[i].tresc_wiadomosci, msg_r.msg);
                                                 tablica_wiadomosci[i].typ_wiadomosci = msg_r.typ_wiadomosci;
                                                 tablica_wiadomosci[i].priorytet = msg_r.priorytet;
                                                 tablica_wiadomosci[i].nr_wiadomosci = i+1;
                                                 last = i+1;
                                                 break;
                                        }
                                }

                                wysylanie_asynchroniczne(msg_r.typ_wiadomosci,message,msg_r.msg);


                               int id_nadawcy,dont_send=0;
                                for(int i=0;i<30;i++){
                                        mes2 = reset;
                                        if(klienci_info[i].czy_czeka==1){
                                                if(klienci_info[i].typ_wiadomosci==msg_r.typ_wiadomosci){
                                                        for(int i=0;i<16;i++){
                                                                if(baza_nadawcow[i].typ_wiadomosci==msg_r.typ_wiadomosci){

                                                                        id_nadawcy =baza_nadawcow[i].nadawca;
                                                                        break;
                                                                }
                                                        }

                                                        for(int g=0;g<MAX;g++){
                                                                if(dont_receive[g].id_nadawcy==id_nadawcy && dont_receive[g].id_odbiorcy==msg_r.id){
                                                                        dont_send=1;
                                                                        break;

                                                                }
                                                        }
                                                        if(dont_send==0){
                                                        int id_k = klienci_info[i].id_kolejki;
                                                        strcpy(mes2.msg,msg_r.msg);
                                                        mes2.end=1;
                                                        mes2.mtype = 8;
                                                        message.id = msg_r.id;
                                                        mes2.priorytet = msg_r.priorytet;
                                                        send_message(mes2,id_k);
                                                        klienci_info[i].czy_czeka=0;
                                                        if(klienci_info[i].liczba_wiadomosci>0){
                                                                klienci_info[i].liczba_wiadomosci--;
                                                        }
                                                        for(int j=0;j<30;j++){
                                                                if(klienci_info[i].wiadomosci_wyslane[j]==0){

                                                                        klienci_info[i].wiadomosci_wyslane[j]=last;
                                                                        break;
                                                                }

                                                        }
                                                }
                                        }
                                }
                        }



                        message.mtype= 5;
                         
                        strcpy(message.msg,"Wiadomosc zostala pomyslnie dodana do bazy");
                 
                        send_message(message,qid);


                }


            }
            else if(msg_r.option==5){

                        int flg=0;
                        bool control=true;
                        message = reset;
                        int liczba_wiad=0;

                        for(int i=0;i<16;i++){
                                 if(users[i].id==msg_r.id){
                                        control=false;
                                        break;

                                }
                        }
                        for(int i=0;i<16;i++){
                                if(odbiorcy[i].typ_wiadomosci==msg_r.typ_wiadomosci && odbiorcy[i].id == msg_r.id){
                                        if(odbiorcy[i].liczba_wiadomosci!=0){
                                                liczba_wiad = odbiorcy[i].liczba_wiadomosci;
                                        }

                                        flg=1;
                                        break;

                                }
                        }

                        if(flg==0){

                                strcpy(message.msg, "Uzytkownik nie subskrybuje danego tematu");
                        }

                        if(control){

                                strcpy(message.msg,"Uzytkownik nie jest zalogowany");

                        }

                        if(control || flg==0){

                                
                                if(msg_r.odbior==1){
                                        message.flag=1;
                                        message.end=1;
                                        message.mtype=8;
                                        send_message(message,qid);
                                }
                                else if(msg_r.odbior==2){
                                        message.flag=0;
                                        message.mtype=7;
                                        send_message(message,qid);
                                }
                 

                        }
                        else if(!control && flg==1){

                                for(int j=0;j<30;j++){
                                                if(klienci_info[j].typ_wiadomosci==0){
                                
                                                        klienci_info[j].id = msg_r.id;
                                                        klienci_info[j].id_kolejki = msg_r.id_kolejki;
                                                        klienci_info[j].typ_wiadomosci = msg_r.typ_wiadomosci;
                                                   
                                                        klienci_info[j].subskrypcja = msg_r.odbior;
                                                   
                                                        if(liczba_wiad!=0){
                                                                klienci_info[j].liczba_wiadomosci = liczba_wiad;
                                                                klienci_info[j].typ_subskrypcji=2;
                                                        }
                                                        else if(liczba_wiad==0){
                                                                klienci_info[j].typ_subskrypcji=1;
                                                        }
                                                        break;


                                                }
                                        }
                            if(msg_r.odbior==1){ //client receives synchroniously 
                                   sprawdzanie(msg_r.typ_wiadomosci,message,qid,msg_r.id);
                            }

                    }

            }
            else if(msg_r.option==6){

                        bool control=true;
                        message = reset;

                if(msg_r.flag==1){
                        for(int i=0;i<16;i++){
                                 if(users[i].id==msg_r.id){
                                        control=false;
                                        break;

                                }
                        }



                        if(control){

                                strcpy(message.msg,"Uzytkownik nie jest zalogowany");
                                message.mtype=11;
                                message.end=1;
                                message.flag=1;

                        }
                        else if(!control){
                                for(int i=0;i<MAX;i++){
                                        message = reset;
                                        if(users[i].id!=0){
                                                if(users[i].id!=msg_r.id){
                                                        message.flag=0;
                                                        strcpy(message.name,users[i].name);
                                                        message.id = users[i].id;
                                                        message.end=0;
                                                        message.mtype=11;
                                                        send_message(message,qid);
                                                }
                                        }
                                        else if(users[i].id==0){
                                                message.flag=1;
                                                strcpy(message.msg,"Powyżej zostały przesłane nazwy zalogowanych użytkowników");
                                                message.end=1;
                                                message.mtype=11;
                                                send_message(message,qid);
                                                break;
                                                }
                                }
                        }

                }
                if(msg_r.flag==2){
                        int id,marked=0;
                        message = reset;
                        for(int i=0;i<MAX;i++){
                                if(strcmp(users[i].name,msg_r.name)==0){
                                        id = users[i].id;
                                        marked=1;
                                        break;
                                }
                        }
                        if(marked==0){
                                strcpy(message.msg,"Nie ma zalogowanego użytkownika o podanej nazwie");
                                message.flag=0;
                                message.mtype=7;
                                send_message(message,qid);
                        }
                        if(marked==1){
                        for(int i=0;i<MAX;i++){
                                if(dont_receive[i].id_nadawcy==0){
                                        dont_receive[i].id_odbiorcy=msg_r.id;
                                        dont_receive[i].id_nadawcy=id;
                                        break;

                                }
                        }
                    }

                }

            }


        }

    return 0;
}
