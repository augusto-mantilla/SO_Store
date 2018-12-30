//Defenition of variables that can be converted to .conf file

#define numberProductsPerShelf 200 
#define numberOfShelfs 1
#define fillShelfWhenLess 20
#define maxProducers 5


//Imports:
    #include <stdio.h>
    #include <pthread.h> 
    #include <semaphore.h>


//Variables:
    int shelfs[numberOfShelfs][numberProductsPerShelf];
    int position[numberOfShelfs];
    int shelfsNeedingToFill = numberProducersActive = 0;

//threads:
    pthread_t threadController; 
    pthread_t threadProducer[maxProducers];

//Sync variables
   pthread_mutex_t needingToFill;
   pthread_mutex_t producersActive;
//    sem_t readyToConsume;

// client(){
//     sem_cli(wait);
//     close(lockP);
//     if(consuption())
//         prodToReplace++;
//     if (prodToReplace >= N/10)
//         sem_control(post);
//     open(lockP);
// }

// producer(){
//     sem_prod(wait);
//     fill();
//     prodToReplace = 0;
//     sem_control(post);
// }

void *controller(void *unused){
    int i, j;
    int prodToReplace = 0;
    int numberOfWorkers = 1;         
    int numberOfCycles = 0;          
    while(1){
        for (i = 0; i < numberOfShelfs; i++){
            for(j = 0; j < numberProductsPerShelf; j++){
                if(shelfs[i][j] == 0)
                prodToReplace++;
            }
            if (prodToReplace <= fillShelfWhenLess){
                pthread_mutex_lock(&needingToFill);
                shelfsNeedingToFill++;
                pthread_mutex_unlock(&needingToFill);
                pthread_mutex_lock(&producersActive);
                if (numberProducersActive < numberOfWorkers){       //missing defenition on when to kill or add another
                    pthread_mutex_unlock(&producersActive);
                    pthread_create(&threadProducer, NULL, producer, NULL)
                    }
                }
            }
        }

        if (numberOfCycles == 3){
            numberOfCycles = 0;
            if (shelfsNeedingToFill/numberOfWorkers >= numberOfShelfs*0.1)
                numberOfWorkers++;
            if (shelfsNeedingToFill/numberOfWorkers <= numberOfShelfs*0.05 && numberOfWorkers > 1)
                numberOfWorkers--;    
        }
        sleep(10);
        numberOfCycles++;
    }
}

void consumeProduct(int shelf){
    shelfs[shelf][position[shelf]] = 0;
    position[shelf] = (position[shelf] + 1) % numberProductsPerShelf;
}

void fillshelf(int shelf){
    int i;
    for(i=0; i<numberProductsPerShelf; i++)
        if (shelfs[shelf][i] == 0)
            shelfs[shelf][i] = 1;        
}

int startShelfManagement(){
    int i;
    for(i=0; i<numberOfShelfs; i++){
        position[i] = 0;
        fillshelf(i);
    }
    
    if(pthread_create(&threadController, NULL, controller, NULL) != 0){
        printf("Controller not working properly!\n");
        return 0;
    }
    return 1;
}

/**
 * The goal of this function is to show how the to init all shelfs 
 */
int main(){ 
    if(startShelfManagement())
        printf("Shelfs are ready to work");
    else 
        printf("You should abort, something is wrong!");
    printf("\n");
	return 0;
}	

