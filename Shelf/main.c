//Defenition of variables that can be converted to .conf file

#define numberProductsPerShelf 20
#define numberOfShelfs 26
#define fillShelfWhenLess 15
#define maxProducers 5
#define clientSimulation 500


//Imports:
    #include <stdio.h>
    #include <pthread.h> 
    #include <semaphore.h>
    #include <unistd.h>
    #include <stdlib.h>

//Functions:
    int startShelfManagement();
    void *client (void *numberShelf);
    void *producer(void *numberShelf);
    void *controller(void *unused);
    void consumeProduct(int shelf);
    void fillShelf(int shelf);

//Variables:
    int shelfs[numberOfShelfs][numberProductsPerShelf];
    int position[numberOfShelfs];
    int shelfsNeedingToFill = 0;
    int numberProducersActive = 0;

//threads:
    pthread_t threadController; 
    pthread_t threadProducer[maxProducers];
    pthread_t threadClient[clientSimulation];

//Sync variables
    pthread_mutex_t needingToFill;
    pthread_mutex_t producersActive;
    pthread_mutex_t shelfLock[numberOfShelfs];
    sem_t semProd[numberOfShelfs];


/**
 * The goal of this function is to show how the to init all shelfs and how the client should behave 
 */
int main(){ 
    if(startShelfManagement())
        printf("Shelfs are ready to work");
    else 
        printf("You should abort, something is wrong!");
    printf("\n");


    int i;
    int id = 0;
    for (i=0;i<clientSimulation;i++){
        int random = rand() % numberOfShelfs;
        if (pthread_create(&threadClient[id], NULL, client, &random) != 0) {
            printf("erro na criacao do cliente\n");
        }
        else{
            printf("im a new client id: %d consuming %d \n", id, random);
            id++;
            sleep(1);
        }
    }
    for (i=0;i<clientSimulation;i++){
        pthread_join(threadClient[i],NULL);
    }
	return 0;
}	


int startShelfManagement(){
    int i;
    pthread_mutex_init(&needingToFill, NULL);
    pthread_mutex_init(&producersActive, NULL);
       
    for(i=0; i<numberOfShelfs; i++){
        pthread_mutex_init(&shelfLock[i], NULL);
        position[i] = 0;
        sem_init(&semProd[i], 0, 0);
        fillShelf(i);
    }

    if(pthread_create(&threadController, NULL, controller, NULL) != 0){
        printf("Controller not working properly!\n");
        return 0;
    }
    return 1;
}


void *client (void *numberShelf){
    int shelf = *((int *) numberShelf);
    sem_wait(&semProd[shelf]);
    pthread_mutex_lock(&shelfLock[shelf]);
    consumeProduct(shelf);  
    pthread_mutex_unlock(&shelfLock[shelf]);
}


void *producer(void *numberShelf){
    int shelf = *((int *) numberShelf);
    pthread_mutex_lock(&producersActive);
        printf("refilling... \t\t\t\t shelf: %d \n", shelf);
        numberProducersActive++;
    pthread_mutex_unlock(&producersActive);

    pthread_mutex_lock(&shelfLock[shelf]);
        fillShelf(shelf);
    pthread_mutex_unlock(&shelfLock[shelf]);
    
    pthread_mutex_lock(&needingToFill);
        shelfsNeedingToFill--;
    pthread_mutex_unlock(&needingToFill);
    
    sleep(10);                                                                           //Sleep

    pthread_mutex_lock(&producersActive);
        numberProducersActive--;
    pthread_mutex_unlock(&producersActive);
    pthread_exit(NULL);
}


void *controller(void *unused){
    int shelf, j, prodToReplace;
    int numberOfWorkers = 1;         
    int producerCounter = 0;
    int numberOfCycles = 0;      
    while(1){
        printf("\nlets see if we need to produce\n");
        for (shelf = 0; shelf < numberOfShelfs; shelf++){
            prodToReplace = 0;
            for(j = 0; j < numberProductsPerShelf; j++){
                if(shelfs[shelf][j] == 0){
                    prodToReplace++;
                }
            }
            if (numberProductsPerShelf - prodToReplace <= fillShelfWhenLess){
                pthread_mutex_lock(&needingToFill);
                    shelfsNeedingToFill++;
                pthread_mutex_unlock(&needingToFill);
                if (numberProducersActive < numberOfWorkers){ 
                    int temp = shelf; // this temporary variable prevents that shelf value swaps while the producer is waiting to refill
                    pthread_create(&threadProducer[producerCounter], NULL, producer, &temp);
                    producerCounter = (producerCounter + 1) % maxProducers;                     //prevents the same thread creation twice in a row
                }
            }
        }
        if (numberOfCycles == 3){
            numberOfCycles = 0;
            if (shelfsNeedingToFill/numberOfWorkers >= numberOfShelfs*0.1){
                printf("Adding a new producer!\n");
                numberOfWorkers++;
            }
            if (shelfsNeedingToFill/numberOfWorkers <= numberOfShelfs*0.05 && numberOfWorkers > 1){
                printf("Removing a new producer!\n");
                numberOfWorkers--;    
            }
        }
        sleep(4);                                                                                           //Sleep
        numberOfCycles++;
    }
}


void consumeProduct(int shelf){
    if (shelfs[shelf][position[shelf]] == 1){
        shelfs[shelf][position[shelf]] = 0;
        position[shelf] = (position[shelf] + 1) % numberProductsPerShelf;
    }
}


void fillShelf(int shelf){
    int i;
    for(i = 0; i < numberProductsPerShelf; i++)
        if (shelfs[shelf][i] == 0){
            shelfs[shelf][i] = 1;  
            sem_post(&semProd[shelf]);
        }      
}



