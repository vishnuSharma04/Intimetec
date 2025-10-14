#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 100
#define MAX_NAME_LENGTH 64 


typedef struct {
    int roll_number;         
    char name[MAX_NAME_LENGTH];
    int marks[3];
    int total_marks;         
    char grade;
} Student;


char calculate_grade(int total_marks) {
    if (total_marks >= 270) return 'A';
    if (total_marks >= 240) return 'B';
    if (total_marks >= 210) return 'C';
    if (total_marks >= 180) return 'D';
    return 'F';
}


void print_grade_description(char grade) {
    switch (grade) {
        case 'A':
            puts("Grade: A — Excellent");
            break;
        case 'B':
            puts("Grade: B — Good");
            break;
        case 'C':
            puts("Grade: C — Satisfactory");
            break;
        case 'D':
            puts("Grade: D — Needs Improvement");
            break;
        default: // Handles 'F' and any other case.
            puts("Grade: F — Fail");
            break;
    }
}

int main(void) {
    Student students[MAX_STUDENTS];
    int num_students;

    printf("Enter the number of students: ");
 
    if (scanf("%d", &num_students) != 1) {
        fprintf(stderr, "Invalid input for number of students.\n");
        return 1;
    }
    if (num_students <= 0 || num_students > MAX_STUDENTS) {
        fprintf(stderr, "Number of students must be between 1 and %d\n", MAX_STUDENTS);
        return 1;
    }

    printf("Enter student details (roll name marks1 marks2 marks3):\n");
    for (int i = 0; i < num_students; ++i) {
      
        int current_roll_number, marks1, marks2, marks3;
        char current_name[MAX_NAME_LENGTH];


        if (scanf("%d %63s %d %d %d", &current_roll_number, current_name, &marks1, &marks2, &marks3) != 5) {     //this question was last asked to me , this is to check if user has exactly inputted 5 inputs as asked by the program else it will throw err.
            fprintf(stderr, "Error: Invalid input format for student %d\n", i + 1);
            return 1;
        }
        
     
        if (current_roll_number <= 0 || marks1 < 0 || marks1 > 100 || marks2 < 0 || marks2 > 100 || marks3 < 0 || marks3 > 100) {  //this part  if statement is to check if all the marks have been correctly entered.
            fprintf(stderr, "Error: Invalid data for student %d (check roll>0 and marks 0-100)\n", i + 1);
            return 1;
        }

        
        for (int j = 0; j < i; ++j) {
            if (students[j].roll_number == current_roll_number) { //as mentioned in the comments i have added conditions to check the dulicate rollno
                fprintf(stderr, "Error: Duplicate roll number %d found for student %d\n", current_roll_number, i + 1);
                return 1;
            }
        }

        
        students[i].roll_number = current_roll_number;
        students[i].name[MAX_NAME_LENGTH - 1] = '\0';
        students[i].marks[0] = marks1;
        students[i].marks[1] = marks2;
        students[i].marks[2] = marks3;
        students[i].total_marks = marks1 + marks2 + marks3;
        students[i].grade = calculate_grade(students[i].total_marks);
    }

   
    printf("\n Student Performance Report ");
    for (int i = 0; i < num_students; ++i) {
        printf("Roll: %d\n", students[i].roll_number);
        printf("Name: %s\n", students[i].name);
        printf("Marks: %d %d %d\n", students[i].marks[0], students[i].marks[1], students[i].marks[2]);
        printf("Total: %d\n", students[i].total_marks);
        print_grade_description(students[i].grade);
        
    }

    return 0;
}
