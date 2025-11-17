#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Players_data.h"

typedef enum {
    Batsman,
    Bowler,
    Allrounder
} Role;


typedef struct Team Team;
typedef struct MyPlayer MyPlayer;

struct MyPlayer {
    int id;
    char playername[50];
    Role role;
    int Totalruns;
    float battingavg;
    float strikerate;
    int wickets;
    float economyrate;
    float perfindex;
    char teamName[50];

    struct MyPlayer* next;
};


struct Team {
    int TeamId;
    char Name[50];
    int TotalPlayers;
    float AvgBattingStrikeRate;
    MyPlayer* batsmen_sorted;
    MyPlayer* bowlers_sorted; 
    MyPlayer* allrounders_sorted; 
    
};

Team* teamHead = NULL;
MyPlayer* global_batsmen_sorted = NULL;
MyPlayer* global_bowlers_sorted = NULL;
MyPlayer* global_allrounders_sorted = NULL;

void CreateTeam(const char* teamName, int teamID) {
    Team* newteam = malloc(sizeof(Team));
    if (newteam == NULL) {
        fprintf(stderr, "failed to create a new team\n");
        return;
    }
    newteam->next = NULL;
    newteam->players = NULL;
    newteam->TotalPlayers = 0;
    newteam->AvgBattingStrikeRate = 0.0f;
    strncpy(newteam->Name, teamName, sizeof(newteam->Name) - 1);
    newteam->Name[sizeof(newteam->Name) - 1] = '\0';
    newteam->TeamId = teamID;
    newteam->battingSRSum = 0.0f; 
    newteam->battingPlayerCount = 0; 

    newteam->batsmen_sorted = NULL; 
    newteam->bowlers_sorted = NULL;
    newteam->allrounders_sorted = NULL;

    if (teamHead == NULL) {
        teamHead = newteam;
    } else {
        Team* temp = teamHead;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newteam;
    }
}

void initTeam(void) {
    for (int i = 0; i < teamCount; ++i) {
        int id = 100 + i;
        CreateTeam(teams[i], id);
    }
}

void perfIndex(MyPlayer *p);

void initPlayers(void) {

    for (int i = 0; i < playerCount; ++i) {

        Team* t = teamHead;
        while (t && strcmp(t->Name, players[i].team) != 0) {
            t = t->next;
        }

        if (!t) {
            printf( "warning: team not found for player id \n");
            return;
        }

        MyPlayer* p = malloc(sizeof(MyPlayer));
        if (!p) {
            fprintf(stderr, "malloc failed for player %d\n", players[i].id);
            return; 
        }

        p->id = players[i].id;
        strncpy(p->playername, players[i].name, sizeof(p->playername) - 1);
        p->playername[sizeof(p->playername) - 1] = '\0';

        strncpy(p->teamName, t->Name, sizeof(p->teamName) - 1);
        p->teamName[sizeof(p->teamName) - 1] = '\0';

        
        if (strcasecmp(players[i].role, "Batsman") == 0) {
            p->role = Batsman;
        } else if (strcasecmp(players[i].role, "Bowler") == 0) {
            p->role = Bowler;
        } else {
            p->role = Allrounder;
        }

        p->Totalruns = players[i].totalRuns;
        p->battingavg = players[i].battingAverage;
        p->strikerate = players[i].strikeRate;
        p->wickets = players[i].wickets;
        p->economyrate = players[i].economyRate;
        p->next = NULL;

        perfIndex(p);

        if (p->role == Batsman) {
            insertsortedplayer(&t->batsmen_sorted, p);
            insertsortedplayer(&global_batsmen_sorted, p);
        } else if (p->role == Bowler) {
            insertsortedplayer(&t->bowlers_sorted, p);
            insertsortedplayer(&global_bowlers_sorted, p);
        } else { 
            insertsortedplayer(&t->allrounders_sorted, p);
            insertsortedplayer(&global_allrounders_sorted, p);
        }
        

        t->TotalPlayers += 1;
    }


    for (Team* t = teamHead; t != NULL; t = t->next) {
        
        // Reset/initialize the sum and count fields
        t->battingSRSum = 0.0f;
        t->battingPlayerCount = 0;

        // Loop through batsmen and add their strike rate
        for (MyPlayer* p = t->batsmen_sorted; p != NULL; p = p->next) {
            t->battingSRSum += p->strikerate;
            t->battingPlayerCount++;
        }
        
        // Loop through all-rounders and add their strike rate
        for (MyPlayer* p = t->allrounders_sorted; p != NULL; p = p->next) {
            t->battingSRSum += p->strikerate;
            t->battingPlayerCount++;
        }

        // Calculate the final average, avoiding division by zero
        if (t->battingPlayerCount > 0) {
            t->AvgBattingStrikeRate = t->battingSRSum / t->battingPlayerCount;
        } else {
            t->AvgBattingStrikeRate = 0.0f;
        }
    }
}

void perfIndex(MyPlayer *p)
{
    if (p->role == Batsman)
    {
        p->perfindex = (p->battingavg * p->strikerate) / 100.0f;
    }
    else if (p->role == Bowler)
    {
        p->perfindex = (p->wickets * 2.0f) + (100.0f - p->economyrate);
    }
    else
    {
        p->perfindex = (p->battingavg * p->strikerate) / 100.0f + (p->wickets * 2.0f);
    }
}


void PrintTeamsSummary(void) {
    for (Team* t = teamHead; t != NULL; t = t->next) {
        printf("TeamID=%d Name='%s' Players=%d AvgSR=%.2f\n",t->TeamId, t->Name, t->TotalPlayers, t->AvgBattingStrikeRate);
    }
}


void PrintPlayersOfTeam(int teamId) {
    Team* t = teamHead;
    while (t && t->TeamId != teamId) t = t->next;
    if (!t) { printf("Team with ID %d not found.\n", teamId); return; }
    printf("Players of Team %s (ID %d):\n", t->Name, t->TeamId);
    for (MyPlayer* p = t->players; p != NULL; p = p->next) {
        const char *role_s = (p->role == Batsman) ? "Batsman" : (p->role == Bowler) ? "Bowler" : "All-rounder";
        printf("%d\t%-20s %s  Runs=%d SR=%.1f PI=%.2f\n",
               p->id, p->playername, role_s, p->Totalruns, p->strikerate, p->perfindex);
    }
}

void insertsortedplayer(MyPlayer **head, MyPlayer *x) {

    if (*head == NULL) {
        *head = x;
        x->next = NULL;
        return;
    }
    if (x->perfindex >= (*head)->perfindex) {
        x->next = *head;
        *head = x;
        return;
    }
    MyPlayer *temp = *head;

    while (temp->next != NULL && temp->next->perfindex >= x->perfindex) {
        temp = temp->next;
    }
    x->next = temp->next;
    temp->next = x;
}

void addnewplayer(void) {
    char line[256];

    printf("Enter the team ID: ");
    if (!fgets(line, sizeof(line), stdin)) {
        printf("Input error\n");
        return;
    }
    int checkID = 0;
    if (sscanf(line, "%d", &checkID) != 1) {
        printf("Invalid team ID\n");
        return;
    }


    Team* IDcheck = teamHead;
    while (IDcheck != NULL && IDcheck->TeamId != checkID) {
        IDcheck = IDcheck->next;
    }
    if (IDcheck == NULL) {
        printf("ID not found\n");
        return;
    }


    MyPlayer* newPlayer = malloc(sizeof(MyPlayer));
    if (!newPlayer) {
        printf("malloc failed\n");
        return;
    }
    newPlayer->next = NULL;


    printf("Enter Player ID: ");
    if (fgets(line, sizeof(line), stdin) && sscanf(line, "%d", &newPlayer->id) != 1) {
        newPlayer->id = 0;
    }

    printf("Enter the player name: ");
    if (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        strncpy(newPlayer->playername, line, sizeof(newPlayer->playername)-1);
        newPlayer->playername[sizeof(newPlayer->playername)-1] = '\0';
    }

    printf("Role 1-Batsman, 2-Bowler, 3-Allrounder: ");
    if (fgets(line, sizeof(line), stdin)) {
        int r = 0;
        if (sscanf(line, "%d", &r) == 1) {
            if (r == 1) newPlayer->role = Batsman;
            else if (r == 2) newPlayer->role = Bowler;
            else newPlayer->role = Allrounder;
        }
    }

    if (newPlayer->role == Batsman || newPlayer->role == Allrounder) {
        printf("Enter the Batting Strike Rate (float): ");
        if (fgets(line, sizeof(line), stdin)) sscanf(line, "%f", &newPlayer->strikerate);

        printf("Enter the Total Runs (int): ");
        if (fgets(line, sizeof(line), stdin)) sscanf(line, "%d", &newPlayer->Totalruns);

        printf("Enter the Batting Average (float): ");
        if (fgets(line, sizeof(line), stdin)) sscanf(line, "%f", &newPlayer->battingavg);
    }

    if (newPlayer->role == Bowler || newPlayer->role == Allrounder) {
        printf("Enter the Total Wickets (int): ");
        if (fgets(line, sizeof(line), stdin)) sscanf(line, "%d", &newPlayer->wickets);

        printf("Enter the Economy Rate (float): ");
        if (fgets(line, sizeof(line), stdin)) sscanf(line, "%f", &newPlayer->economyrate);
    }
    strncpy(newPlayer->teamName, IDcheck->Name, sizeof(newPlayer->teamName) - 1);
    newPlayer->teamName[sizeof(newPlayer->teamName) - 1] = '\0';

    perfIndex(newPlayer);
    if (newPlayer->role == Batsman) {
        insertsortedplayer(&IDcheck->batsmen_sorted, newPlayer);
        insertsortedplayer(&global_batsmen_sorted, newPlayer);
    } else if (newPlayer->role == Bowler) {
        insertsortedplayer(&IDcheck->bowlers_sorted, newPlayer);
        insertsortedplayer(&global_bowlers_sorted, newPlayer);
    } else {
        insertsortedplayer(&IDcheck->allrounders_sorted, newPlayer);
        insertsortedplayer(&global_allrounders_sorted, newPlayer);
    }
    insertsortedplayer(&IDcheck->players, newPlayer);
    IDcheck->TotalPlayers += 1;

    if (newPlayer->role == Batsman || newPlayer->role == Allrounder) {
        IDcheck->battingSRSum += newPlayer->strikerate;
        IDcheck->battingPlayerCount++;

        IDcheck->AvgBattingStrikeRate = IDcheck->battingSRSum / IDcheck->battingPlayerCount;
    }
}

Team* findTeamById(int id) {
    Team* t = teamHead;
    while (t != NULL) {
        if (t->TeamId == id) {
            return t; // Found the team
        }
        t = t->next;
    }
    return NULL; // Team not found
}

Team* findmiddle(Team* a) {
    // Your logic here
    Team* slow = a;
    Team* fast = a;
    while(fast != NULL && fast->next!= NULL){
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
    
}

Team* sortedMerge(Team* a, Team* b) {
    
    if (a == NULL) {
        return b;
    }

    if (b == NULL) {
        return a;
    }
    Team* result = NULL; /
    if (a->AvgBattingStrikeRate >= b->AvgBattingStrikeRate) {
        result = a;.
        result->next = sortedMerge(a->next, b);
        
    } else {
        result = b;
        result->next = sortedMerge(a, b->next);
    }
    return result;
}

void mergeSort(Team** headRef) {
    Team* head = *headRef;
    Team* left;
    Team* right;

    if (head == NULL || head->next == NULL) {
        return;
    }


    Team* middle = findMiddle(head);
    right = middle->next;
    left = head;

    middle->next = NULL;
    mergeSort(&left);
    mergeSort(&right);
    *headRef = sortedMerge(left, right);
}


Team* copyTeamNode(Team* t) {
    if (t == NULL) return NULL;
    
    Team* newNode = (Team*)malloc(sizeof(Team));
    if (newNode == NULL) {
        printf("Error: Malloc failed in copyTeamNode\n");
        return NULL;
    }
    newNode->TeamId = t->TeamId;
    strncpy(newNode->Name, t->Name, sizeof(newNode->Name) - 1);
    newNode->TotalPlayers = t->TotalPlayers;
    newNode->AvgBattingStrikeRate = t->AvgBattingStrikeRate;

    newNode->next = NULL;
    newNode->batsmen_sorted = NULL;
    newNode->bowlers_sorted = NULL;
    newNode->allrounders_sorted = NULL;
    
    return newNode;
}

void freeTeamList(Team* head) {
    Team* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

void displayTeamsBySR() {
    
    Team* sortedHead = NULL;
    Team* sortedTail = NULL;
    
    for (Team* t = teamHead; t != NULL; t = t->next) {
        Team* newNode = copyTeamNode(t);
        if (newNode == NULL) {
            freeTeamList(sortedHead); 
            return;
        }
        
        if (sortedHead == NULL) {
            sortedHead = newNode;
            sortedTail = newNode;
        } else {
            sortedTail->next = newNode;
            sortedTail = newNode;
        }
    }

    mergeSort(&sortedHead);

    printf("\nTeams Sorted by Average Batting Strike Rate (Descending):\n\n");
    printf("%-5s %-20s %-10s %-12s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    Team* current = sortedHead;
    while (current != NULL) {
        printf("%-5d %-20s %-10.2f %-12d\n",current->TeamId,current->Name,current->AvgBattingStrikeRate,current->TotalPlayers);
        current = current->next;
    }
    freeTeamList(sortedHead);
}

void PrintTopK(){
    char line[100];
    int teamID,rolechoice,k;
    printf("Enter the team ID");
    if(!fgets(line,sizeof(line),stdin)) return;
    if(sscanf(line,"%d",&teamID) != 1) return;
    Team* iterator = teamHead;
    while(iterator!=NULL){
        if(iterator->TeamId != teamID){
            iterator = iterator->next;
        }
    }
    printf("Enter the role in Number 1.Batsman 2.Bowler 3.Allrounder : ");
    if(!fgets(line,sizeof(line),stdin)) return;
    if(sscanf(line,"%d",&rolechoice) != 1 || rolechoice > 1) return;
    printf("Enter the top K players needeed to be printed : ");
    if(!fgets(line,sizeof(line),stdin)) return;
    if(sscanf(line,"%d",&k)!= 1) return;
    MyPlayer* ListToPrint = NULL;
    switch (rolechoice)
    {
    case 1:
        ListToPrint = iterator->batsmen_sorted;
        break;
    case 2:
        ListToPrint = iterator->bowlers_sorted;
        break;
    case 3:
        ListToPrint = iterator->allrounders_sorted;
    default:
        break;
    }
    int count = 0;MyPlayer* printplayercount;
    while(ListToPrint != NULL && count > k){
        printf("%-5d %-22s %-12s %-6d %-6.1f %-6.1f %-5d %-5.1f %-8.2f\n",
               printplayercount->id, printplayercount->playername,
               printplayercount->Totalruns, printplayercount->battingavg, printplayercount->strikerate,
               printplayercount->wickets, printplayercount->economyrate, printplayercount->perfindex);
        
        printplayercount = printplayercount->next;
        count++;
    }
}
//  display player by their role of all team 
void displayplayerbyrole(){
    char line[100];
    int rolechoice;
    printf("Enter the role in Number 1.Batsman 2.Bowler 3.Allrounder : ");
    if(!fgets(line,sizeof(line),stdin)) return;
    if(sscanf(line,"%d",&rolechoice) != 1 || rolechoice > 1) return;
    MyPlayer* ListToPrint = NULL;
    switch (rolechoice)
    {
    case 1:
        ListToPrint = global_batsmen_sorted;
        break;
    case 2:
        ListToPrint = global_bowler_sorted;
        break;
    case 3:
        ListToPrint = global_allrounders_sorted;
    default:
        break;
    }
    int count=0;
    MyPlayer* printplayercount;
    while(ListToPrint != NULL){
        printf("%-5d %-22s %-12s %-6d %-6.1f %-6.1f %-5d %-5.1f %-8.2f\n",
               printplayercount->id, printplayercount->playername,
               printplayercount->Totalruns, printplayercount->battingavg, printplayercount->strikerate,
               printplayercount->wickets, printplayercount->economyrate, printplayercount->perfindex);
        count++;
        printplayercount = printplayercount->next;
    }

}
int main(void) {
    initTeam();
    initPlayers();

    int choice = 0;
    char line[100];

   
    while (choice != 6) {
        printf("\n--- ICC ODI Player Performance Analyzer ---\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        if (!fgets(line, sizeof(line), stdin)) {
            break; 
        }
        if (sscanf(line, "%d", &choice) != 1) {
            choice = 0; 
        }

        
        switch (choice) {
            case 1:
                addnewplayer();
                break;
            case 2:
                PrintPlayersOfTeam();
                break;
            case 3:
                displayTeamsBySR();
                break;
            case 4:
                PrintTopK();
                break;
            case 5:
                displayplayerbyrole();
                break;
            case 6:
                printf("Exiting... Freeing all memory.\n");
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 6.\n");
        }
    }
    freeTeamList(); 
    return 0;

}

