#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    int id;
    char* data;
    struct Node* next;
    struct Node* prev;
    struct Node* reference;
} Node;

typedef struct Graph {
    Node* head;
    int size;
} Graph;

Node* createNode(int id, const char* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        perror("Failed to allocate memory for node");
        exit(EXIT_FAILURE);
    }
    
    newNode->id = id;
    newNode->data = strdup(data);
    if (!newNode->data) {
        perror("Failed to allocate memory for data");
        free(newNode);
        exit(EXIT_FAILURE);
    }
    
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->reference = NULL;
    
    return newNode;
}

Graph* createGraph() {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) {
        perror("Failed to allocate memory for graph");
        exit(EXIT_FAILURE);
    }
    
    graph->head = NULL;
    graph->size = 0;
    
    return graph;
}

void addNode(Graph* graph, int id, const char* data) {
    Node* newNode = createNode(id, data);
    
    if (!graph->head) {
        graph->head = newNode;
    } else {
        Node* current = graph->head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
        newNode->prev = current;
    }
    
    graph->size++;
}

void createCyclicReference(Graph* graph, int sourceId, int targetId) {
    Node* source = NULL;
    Node* target = NULL;
    Node* current = graph->head;
    
    while (current) {
        if (current->id == sourceId) {
            source = current;
        }
        if (current->id == targetId) {
            target = current;
        }
        current = current->next;
    }
    
    if (source && target) {
        source->reference = target;
    }
}

void displayGraph(Graph* graph) {
    Node* current = graph->head;
    printf("\nGraph Contents:\n");
    printf("------------------------\n");
    
    while (current) {
        printf("Node ID: %d, Data: %s\n", current->id, current->data);
        if (current->reference) {
            printf("  References Node: %d\n", current->reference->id);
        }
        current = current->next;
    }
    printf("------------------------\n");
}

void correctFreeGraph(Graph* graph) {
    Node* current = graph->head;
    Node* next;
    
    while (current) {
        current->reference = NULL;
        current = current->next;
    }
    
    current = graph->head;
    while (current) {
        next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    
    free(graph);
}

int main() {
    printf("Demonstrating cyclic references in dynamic data structures\n");
    
    Graph* graph = createGraph();
    
    addNode(graph, 1, "First Node");
    addNode(graph, 2, "Second Node");
    addNode(graph, 3, "Third Node");
    addNode(graph, 4, "Fourth Node");
    
    createCyclicReference(graph, 1, 3);
    createCyclicReference(graph, 2, 4);
    createCyclicReference(graph, 3, 1);
    createCyclicReference(graph, 4, 2);
    
    displayGraph(graph);
    
    correctFreeGraph(graph);
    
    printf("\nProgram completed successfully.\n");
    
    return 0;
}
