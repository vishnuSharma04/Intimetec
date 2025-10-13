#include <stdio.h>


int g_studentsProcessed = 0;


struct Student {
    int rollNumber;
    char name[50];
    int marks[3];
    int totalMarks;
    float averageMarks;
    char grade;
};


int calculateTotal(int marks[]);
char assignGrade(float average);
void printPerformance(char grade);
void printRollNumbers(int n);


int main() {
    int numStudents;


    scanf("%d", &numStudents);
    
  
    struct Student students[100];

    
    for (int i = 0; i < numStudents; i++) {
        
        scanf("%d %s %d %d %d", &students[i].rollNumber, &students[i].name, &students[i].marks[0], &students[i].marks[1], &students[i].marks[2]);
        students[i].totalMarks = calculateTotal(students[i].marks);
        students[i].averageMarks = (float)students[i].totalMarks / 3.0;
        students[i].grade = assignGrade(students[i].averageMarks);
        
        
        g_studentsProcessed++;
    }

    for (int i = 0; i < numStudents; i++) {
        
        printf("Roll: %d\n", students[i].rollNumber);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", students[i].totalMarks);
        printf("Average: %.2f\n", students[i].averageMarks);
        printf("Grade: %c\n", students[i].grade);
        
        
        if (students[i].grade == 'F') {
            
            printf("Performance: \n");
            if (i < numStudents - 1) { printf("\n"); } 
            continue; 
        }
        
        
        printf("Performance: ");
        printPerformance(students[i].grade);
        printf("\n");
        
        
        if (i < numStudents - 1) {
            printf("\n");
        }
    }

    
    printf("\nList of Roll Numbers (via recursion): ");
    printRollNumbers(numStudents);
    printf("\n");
    
    return 0;
}



int calculateTotal(int marks[]) {
    return marks[0] + marks[1] + marks[2];
}


char assignGrade(float average) {
    if (average >= 85) return 'A';
    else if (average >= 70) return 'B';
    else if (average >= 50) return 'C';
    else if (average >= 35) return 'D';
    else return 'F';
}

void printPerformance(char grade) {
    switch (grade) {
        case 'A': printf("*****"); break;
        case 'B': printf("****"); break;
        case 'C': printf("***"); break;
        case 'D': printf("**"); break;
    }
}


void printRollNumbers(int n) {
    if (n > 0) {
        printRollNumbers(n - 1);
        printf("%d ", n);
    }
}