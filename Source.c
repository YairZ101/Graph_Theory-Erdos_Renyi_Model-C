#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>


// Create a random number between 0 to 1
double rnd_gnr()
{
	return (double)rand() / (double)RAND_MAX;
}

// A structure to represent a queue.
typedef struct Queue {
	int val;
	struct Queue* next;
} Queue;

// Add a new node to the Queue
void enqueue(Queue** head, int val) {
	Queue* new_node = malloc(sizeof(Queue));
	assert(new_node);

	new_node->val = val;
	new_node->next = *head;

	*head = new_node;
}

// Remove a node from the Queue and return its value
int dequeue(Queue** head) {
	Queue* current, * prev = NULL;
	int retval = -1;

	if (*head == NULL) return -1;

	current = *head;
	while (current->next != NULL) {
		prev = current;
		current = current->next;
	}

	retval = current->val;
	free(current);

	if (prev)
		prev->next = NULL;
	else
		*head = NULL;

	return retval;
}

// Freeing the Queue's memory
void freeQueue(Queue* head) {
	Queue* temp;
	while (head) {
		temp = head->next;
		free(head);
		head = temp;
	}
}


// A structure to represent an adjacency list node
typedef struct AdjListNode {
	int dest;
	struct AdjListNode* next;
} AdjListNode;

// A structure to represent an adjacency list
typedef struct AdjList
{
	AdjListNode* head;
} AdjList;

// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
typedef struct Graph
{
	int V;
	AdjList* array;
} Graph;

// A utility function to create a new adjacency list node
AdjListNode* newAdjListNode(int dest)
{
	AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
	assert(newNode);
	newNode->dest = dest;
	newNode->next = NULL;
	return newNode;
}

// Adds an edge to an undirected graph
void addEdge(Graph* graph, int src, int dest)
{
	// Add an edge from src to dest. A new node is added to the adjacency list of src.
	// The node is added at the beginning.
	AdjListNode* newNode = newAdjListNode(dest);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;

	// Since graph is undirected, add an edge from dest to src also
	newNode = newAdjListNode(src);
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
}

// A utility function that creates a graph of V vertices with probabilty of P
Graph* build_random_graph(int V, float P)
{
	int i, j;

	Graph* graph = (struct Graph*)malloc(sizeof(Graph));
	assert(graph);
	graph->V = V;

	// Create an array of adjacency lists. Size of array will be V
	graph->array = (AdjList*)malloc(V * sizeof(AdjList));
	assert(graph->array);

	// Initialize each adjacency list as empty by making head as NULL
	for (i = 0; i < V; i++) 
		graph->array[i].head = NULL;

	if (P == 0)
		return graph;

	for (i = 0; i < V; i++) {
		for (j = i + 1; j < V; j++) {
			if (P >= rnd_gnr()) {
				addEdge(graph, i, j);
			}
		}
	}

	return graph;
}

// A utility function to print the adjacency list representation of graph
void printGraph(Graph* graph)
{
	int v;
	for (v = 0; v < graph->V; v++)
	{
		AdjListNode* pCrawl = graph->array[v].head;
		printf("\n Adjacency list of vertex %d\n head ", v);
		while (pCrawl)
		{
			printf("-> %d", pCrawl->dest);
			pCrawl = pCrawl->next;
		}
		printf("\n");
	}
}

// Freeing the Graph's memory
void freeGraph(Graph* graph) {
	for (int v = 0; v < graph->V; v++)
	{
		AdjListNode* pCrawl = graph->array[v].head;
		AdjListNode* pCrawlTemp;
		while (pCrawl)
		{
			pCrawlTemp = pCrawl->next;
			free(pCrawl);
			pCrawl = pCrawlTemp;
		}
	}
	free(graph->array);
	free(graph);
}

// Checking the if there is there is an isloated vertex in the graph
int is_isolated(Graph* graph) {
	// Looping over the array and checking if there is a cell that is pointing to a NULL
	for (int i = 0; i < graph->V; i++) {
		if (graph->array[i].head == NULL)
			return 1;
	}
	return 0;
}

// Standard BFS algorithm
int BFS(Graph* graph, int src, bool* visited, int* distance) {
	int vis = 0;
	visited[src] = true;
	Queue* head = NULL;
	enqueue(&head, src);

	// While loop until the Queue is empty
	while (head) {
		vis = dequeue(&head);
		AdjListNode* adj = graph->array[vis].head;
		int i = 0;
		// Looping over the linked list of the visited vertex
		while (adj != NULL) {
			// If we didn't visit the vertex
			if (!visited[adj->dest])
			{
				enqueue(&head, adj->dest);
				visited[adj->dest] = true;
				if (distance) // In case we want to check distance
					distance[adj->dest] = distance[vis] + 1;
			}

			adj = adj->next;
		}
	}

	freeQueue(head);
	// Returning the last visited vertex
	return vis;
}

// Checking if the graph is connected
int connectivity(Graph* graph) {
	int V = graph->V, i;
	bool* visited = calloc(V, sizeof(bool));
	assert(visited);

	BFS(graph, 0, visited, NULL);

	// Checking if we visited all the vertices
	for (i = 0; i < V; i++) {
		if (visited[i] == false) {
			free(visited);
			return 0;
		}
	}

	free(visited);
	return 1;
}

// Checking what is the diameter of the graph
int diameter(Graph* graph) {
	int V = graph->V, i, j, diam = 0, last_vertex = 0;
	bool* visited = calloc(V, sizeof(bool));
	assert(visited);
	int* distance = calloc(V, sizeof(int));
	assert(distance);

	// looping over each vertex
	for (i = 0; i < V; i++) {
		last_vertex = BFS(graph, i, visited, distance);

		// Checking if distance of the last vertex is greater than the current diameter
		if (distance[last_vertex] > diam)
			diam = distance[last_vertex];

		// "Reseting" the arrays
		for (j = 0; j < V; j++) {
			visited[j] = false;
			distance[j] = 0;
		}
	}

	free(visited);
	free(distance);

	return diam;
}

// Checking Threshold #1 and generating a csv file with the results
void threshold1(int V, int NOGraphs) {
	FILE* fpt;
	int i, length, count = 0, isolated = 0, connected = 0;
	double prob;
	double thresh1[10] = { 0.001, 0.002, 0.003, 0.004, 0.005, 0.008, 0.009, 0.01, 0.015, 0.02 };

	length = sizeof(thresh1) / sizeof(thresh1[0]);
	fpt = fopen("Threshold1.csv", "w+");

	for (i = 0; i < length; i++)
	{
		fprintf(fpt, "%.3f,", thresh1[i]);
	}
	fprintf(fpt, "\n");

	for (i = 0; i < length; i++) {
		prob = thresh1[i];
		count = 0;

		for (int i = 0; i < NOGraphs; i++)
		{
			isolated = 0, connected = 0;
			Graph* graph = build_random_graph(V, prob);
			isolated = is_isolated(graph);
			if (!isolated)
				connected = connectivity(graph);
			if (connected)
				count++;
			freeGraph(graph);
		}
		fprintf(fpt, "%f,", (float)count / (float)NOGraphs);
	}

	fclose(fpt);
}

// Checking Threshold #2 and generating a csv file with the results
void threshold2(int V, int NOGraphs) {
	FILE* fpt;
	int i, length, count = 0, isolated = 0, connected = 0, diam = -1;
	double prob;
	double thresh2[10] = { 0.01, 0.02, 0.04, 0.08, 0.1, 0.13, 0.16, 0.2, 0.25, 0.3 };

	length = sizeof(thresh2) / sizeof(thresh2[0]);
	fpt = fopen("Threshold2.csv", "w+");

	for (i = 0; i < length; i++)
	{
		fprintf(fpt, "%.3f,", thresh2[i]);
	}
	fprintf(fpt, "\n");

	for (i = 0; i < length; i++) {
		prob = thresh2[i];
		count = 0;

		for (int i = 0; i < NOGraphs; i++)
		{
			isolated = 0, connected = 0, diam = -1;
			Graph* graph = build_random_graph(V, prob);
			isolated = is_isolated(graph);
			if (!isolated)
				connected = connectivity(graph);
			if (connected)
				diam = diameter(graph);
			if (diam == 2)
				count++;
			freeGraph(graph);
		}
		fprintf(fpt, "%f,", (float)count / (float)NOGraphs);
	}

	fclose(fpt);
}

// Checking Threshold #3 and generating a csv file with the results
void threshold3(int V, int NOGraphs) {
	FILE* fpt;
	int i, length, count = 0, isolated = 0;
	double prob;
	double thresh3[10] = { 0.001, 0.002, 0.003, 0.004, 0.005, 0.008, 0.009, 0.01, 0.015, 0.02 };

	length = sizeof(thresh3) / sizeof(thresh3[0]);
	fpt = fopen("Threshold3.csv", "w+");

	for (i = 0; i < length; i++)
	{
		fprintf(fpt, "%.3f,", thresh3[i]);
	}
	fprintf(fpt, "\n");

	for (i = 0; i < length; i++) {
		prob = thresh3[i];
		count = 0;

		for (int i = 0; i < NOGraphs; i++)
		{
			isolated = 0;
			Graph* graph = build_random_graph(V, prob);
			isolated = is_isolated(graph);
			if (isolated)
				count++;
			freeGraph(graph);
		}
		fprintf(fpt, "%f,", (float)count / (float)NOGraphs);
	}

	fclose(fpt);
}

// Driver program to test above functions
void main()
{
	srand(time(0));

	printf("Checking Threshold #1");
	threshold1(1000, 500);
	printf("Checking Threshold #2");
	threshold2(1000, 500);
	printf("Checking Threshold #3");
	threshold3(1000, 500);;
}