/* Algorithm Design Assignment
 * This program takes 3 files of students, sorts and merges them into a single array.
 * It does this by first bubble sorting each list, then merging them together
 * using the merge part of merge sort.
 * Next, using this single sorted array, it prints off all of the international students.
 * Finally the user is prompted to enter a surname, which the program checks against
 * the list of students using a binary search.
 *
 * Author: Padraig Redmond
 * Date: 11/03/2016
 */

#include <stdio.h>

#define FUNDSIZE 20
#define NONFUNDSIZE 6
#define INTERSIZE 10
#define TOTALSIZE FUNDSIZE+NONFUNDSIZE+INTERSIZE

typedef enum {FUNDED, NONFUNDED, INTERNATIONAL} group_type;

typedef struct student
{
	char firstname[21];
	char surname[21];
	group_type group;
} Student;

void flush_buffer();
void read_file(Student students[], FILE*, group_type);
void bubble_sort(Student arr[], int size);
void merge(Student arr1[], int size1, Student arr2[], int size2, Student combine[]);
void output_students(Student arr[], FILE* f, int size);
void print_by_group(Student arr[], int size, group_type group);
int search_name(Student arr[], int low, int high, char key[]);

int main()
{
	FILE *f;
	Student funded_students[FUNDSIZE];
	Student nonfunded_students[NONFUNDSIZE];
	Student international_students[INTERSIZE];
	Student temp_combine[NONFUNDSIZE + INTERSIZE];
	Student combined_students[TOTALSIZE];
	int i, j;
	char search_key[21];	// Stores the name of the current search
	int search_result;	// Used to determine if the result of a search was successful
	char answer;	// Used for checking if user wants to search for another name

	// Read in the funded students	
	if(!(f = fopen("unsorted/funded.csv", "r")))
	{
		printf("File not found, exiting\n");
		return 1;
	}	
	read_file(funded_students, f, FUNDED);
	fclose(f);

	// Read in the unfunded students	
	if(!(f = fopen("unsorted/nonfunded.csv", "r")))
	{
		printf("File not found, exiting\n");
		return 1;
	}	
	read_file(nonfunded_students, f, NONFUNDED);
	fclose(f);

	// Read in the international students	
	if(!(f = fopen("unsorted/international.csv", "r")))
	{
		printf("File not found, exiting\n");
		return 1;
	}	
	read_file(international_students, f, INTERNATIONAL);
	fclose(f);

	// Sort the 3 sets of students independently
	bubble_sort(funded_students, FUNDSIZE);
	bubble_sort(nonfunded_students, NONFUNDSIZE);
	bubble_sort(international_students, INTERSIZE);

	// Now use the merge part of merge sort to combine the 3 arrays into one
	//
	// First combining the international and non funded into a temp array
	merge(nonfunded_students, NONFUNDSIZE, international_students, INTERSIZE, temp_combine);

	// Then combine the temp array with the funded students
	merge(funded_students, FUNDSIZE, temp_combine, NONFUNDSIZE+INTERSIZE, combined_students);

	// Output the sorted students to file
	if(!(f = fopen("sorted.txt", "w")))
	{
		printf("File not created, exiting\n");
		return 1;
	}
	output_students(combined_students, f, TOTALSIZE);
	fclose(f);

	// Print all students after sorting
	printf("\n~~~~~~~~~~~~~~~~~~~\n");
	printf("All students sorted\n");
	printf("~~~~~~~~~~~~~~~~~~~\n\n");
	for (i = 0; i < TOTALSIZE; i++)
	{
		printf("%s %s\n", combined_students[i].firstname, combined_students[i].surname);
		// This printf includes the group number as well
		// Printf("%s %s %d\n", combined_students[i].firstname, combined_students[i].surname, combined_students[i].group);	
	}

	printf("\nPress enter to continue...");
	getchar();

	// Print all the international students
	print_by_group(combined_students, TOTALSIZE, INTERNATIONAL);

	// Search for a user by surname
	do
	{
		printf("\nSearch for name: ");
		scanf("%20s", search_key);
		flush_buffer();

		search_result = search_name(combined_students, 0, TOTALSIZE-1, search_key);

		if (search_result == -1)
		{
			printf("\n%s not found\n\n", search_key);
		}
		else
		{
			printf("\n%s found at position %d\n\n", search_key, search_result);
		}
		
		printf("Search again Y/n? ");
		answer = getchar();
		flush_buffer();
	}
	while(answer == 'y' || answer == 'Y');

	return 0;
}

// Flushes any extra input from stdin
void flush_buffer()
{
	while (getchar() != '\n');
}

// Read contents of a file and store in a Student struct
void read_file(Student students[], FILE *f, group_type group)
{
	int i, j;
	int curStudent = 0;
	char buffer[50];

	while (fgets(buffer, 50, f) != NULL)
	{
		i = 0;
		j = 0;
		// Parse first name from buffer
		while (buffer[i] != ',')
		{
			students[curStudent].firstname[j] = buffer[i];
			i++;
			j++;
		}
		students[curStudent].firstname[j] = '\0';	// Terminate the firstname string

		i++;	// Iterate past the semicolon
		j = 0;
		// Parse second name from buffer
		while (buffer[i] != '\n')
		{
			students[curStudent].surname[j] = buffer[i];
			i++;
			j++;
		}
		students[curStudent].surname[j] = '\0';	// Terminate the surname string

		students[curStudent].group = group;
		curStudent++;
	}
}

// Bubble sort which uses a swap variable to terminate early
// if the array is sorted
void bubble_sort(Student arr[], int size)
{
	int i = 0, j = 0;
	int swap;
	Student temp;

	for (i = 0; i < size; i++)
	{
		swap = 0;
		for (j = 0; j < size-1-i; j++)
		{
			if (strcmp(arr[j].surname,arr[j+1].surname) > 0)
			{
				// Swap the two elements
				temp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = temp;

				swap = 1;
			}
		}

		// If no swaps were made, the list is sorted
		if (swap == 0)
		{
			return;
		}
	}
}

// Merge part of merge sort
void merge(Student arr1[], int size1, Student arr2[], int size2, Student combine[])
{
	int head1 = 0, head2 = 0;
	int i = 0;

	// Copy the lowest element of the 2 arrays until an array is empty
	while (head1 < size1 && head2 < size2)
	{
		if (strcmp(arr1[head1].surname, arr2[head2].surname) < 0)
		{
			combine[i] = arr1[head1];
			head1++;
		}
		else
		{
			combine[i] = arr2[head2];
			head2++;
		}
		i++;
	}

	// Copy the rest of the non-empty array
	if (head1 == size1)
	{
		while (head2 < size2)
		{
			combine[i] = arr2[head2];
			i++;
			head2++;
		}
	}
	else
	{
		while (head1 < size1)
		{
			combine[i] = arr1[head1];
			i++;
			head1++;
		}
	}
}

// Output the sorted students to file
void output_students(Student arr[], FILE* f, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		fprintf(f, "%s %s\n", arr[i].firstname, arr[i].surname);
	}

}

// Print all students from a particular group
void print_by_group(Student arr[], int size, group_type group)
{
	int i;

	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Printing all students from ");
	switch(group)
	{
		case FUNDED:
			printf("funded");
			break;
		case NONFUNDED:
			printf("non-funded");
			break;
		case INTERNATIONAL:
			printf("international");
			break;
	}
	printf(" group\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	for (i = 0; i < size; i++)
	{
		if (arr[i].group == group)
		{
			printf("%s %s\n", arr[i].firstname, arr[i].surname);
			// this printf includes the group number 
			// printf("%s %s %d\n", arr[i].firstname, arr[i].surname, arr[i].group);
		}
	}
}

// Recursive binary search for a user by surname, returns the index if found, otherwise -1
int search_name(Student arr[], int low, int high, char key[])
{
	int mid = (low + high) / 2;	

	// Base case: Name found
	if (strcmp(arr[mid].surname, key) == 0)
	{
		return mid;
	}
	else
	{
		// Base case: Finished searching array
		if (low == high)
		{
			return -1;
		}
		else
		{
			if (strcmp(arr[mid].surname, key) < 0)
			{
				search_name(arr, mid+1, high, key);
			}
			else
			{
				search_name(arr, low, mid-1, key);
			}
		}
	}	
}
