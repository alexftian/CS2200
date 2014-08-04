/*
 * student.c
 * Multithreaded OS Simulation for CS 2200, Project 4
 *
 * This file contains the CPU scheduler for the simulation.  
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "os-sim.h"
#include "student.h"

/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t current_mutex;
static pthread_mutex_t ready_mutex;
static pthread_cond_t is_empty;
//static pcb_t *ready_queue;
static pcb_t *head;


int size() {
	int i = 0;
	//int j = 0;
	pcb_t *temp;
	if(head == NULL) return 0;
	for(temp = head; temp->next != NULL; temp = temp->next) {
		printf("%s / ", temp->name);
		i++;
	}
	printf("\n");
	
	return i;
}

void* add_ready_queue(void* void_pcb) {
	assert(void_pcb != NULL);

	pcb_t* newQ = (pcb_t*) void_pcb;
	newQ->next = NULL;
printf("%s\n",newQ->name);
	if(head == NULL) {
//printf("head is null\n");
		pthread_mutex_lock(&ready_mutex);
		head = newQ;
printf("head!! %s\n",head->name);
		pthread_mutex_unlock(&ready_mutex);
	} else {
printf("head is not null\n");
		pcb_t *temp;
		pthread_mutex_lock(&ready_mutex);
		for(temp = head; temp->next != NULL; temp = temp->next);
		temp->next = newQ;
		pthread_mutex_unlock(&ready_mutex);
	}
	pthread_exit(NULL);
}



void* remove_ready_queue(void* void_pcb) {
//printf("remove\n");
	pcb_t *temp;
	//pcb_t *newQ = (pcb_t*)void_pcb;

	if(1) {
		pthread_mutex_lock(&ready_mutex);
		void_pcb = (void*)head;
		//pthread_cond_wait(&
		if(head!=NULL){
			printf("removing : %d\n", head->state);//((pcb_t*)void_pcb)->name);
			head = head->next;
		}	
		pthread_mutex_unlock(&ready_mutex);
	} else {
		pthread_mutex_lock(&ready_mutex);
		for(temp = head; temp->next != NULL; temp = temp->next);
				temp->next = temp->next->next;	
		pthread_mutex_unlock(&ready_mutex);
	}
		
	
	pthread_exit(void_pcb); // fix here again
	//return NULL;
}	






/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *	The current array (see above) is how you access the currently running process indexed by the cpu id. 
 *	See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more information 
 *	about it and its parameters.
 */
static void schedule(unsigned int cpu_id)
{
//printf("schedule\n");
	//int rc = 0;
	pthread_t thread;
	pcb_t* rmv_pcb = NULL;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_create(&thread, &attr, remove_ready_queue, (void*)rmv_pcb); 
	pthread_attr_destroy(&attr);


	if(rmv_pcb != NULL) {
//printf("rmv_pcb isn't null\n");
		rmv_pcb->state = PROCESS_RUNNING;
		context_switch(cpu_id, rmv_pcb, -1); // change the time

		pthread_mutex_lock(&current_mutex);
		current[cpu_id] = rmv_pcb;
		pthread_mutex_unlock(&current_mutex);
	printf("current!! %s\n", current[0]->name);
	} else {
//printf("rmv_pcb is null\n");
		context_switch(cpu_id, NULL, -1);
	}
		
}


/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id)
{
//printf("idle\n");

	pthread_mutex_lock(&ready_mutex);
	while(head == NULL)
		pthread_cond_wait(&is_empty, &ready_mutex);
	pthread_mutex_unlock(&ready_mutex);
	schedule(cpu_id);
	



    /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
    //mt_safe_usleep(1000000);
}


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 */
extern void preempt(unsigned int cpu_id)
{
printf("preempt\n");
    /* FIX ME */
}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{
printf("yield\n");
	current[cpu_id]->state = PROCESS_WAITING;
	schedule(cpu_id);	

}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
printf("terminate\n");
	current[cpu_id]->state = PROCESS_TERMINATED;
	schedule(cpu_id);
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is static priority, wake_up() may need
 *      to preempt the CPU with the lowest priority process to allow it to
 *      execute the process which just woke up.  However, if any CPU is
 *      currently running idle, or all of the CPUs are running processes
 *      with a higher priority than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *	To preempt a process, use force_preempt(). Look in os-sim.h for 
 * 	its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{
//	printf("size is %d\n",size());

	pthread_mutex_lock(&current_mutex);
	process->state = PROCESS_READY;
	pthread_mutex_unlock(&current_mutex);

	pthread_t thread;
printf("wakeup : add\n");
	pthread_create(&thread, NULL, add_ready_queue, (void*)process);
	pthread_cond_signal(&is_empty);
	
/*
	if(static prioiry) {
		added = add_current_priority(process) // make this
			//check if idle, or it's the lowest one
		if(!addded)
			do nothing
		
*/

		// use force preempt

}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -r and -p command-line parameters.
 */
int main(int argc, char *argv[])
{
    int cpu_count;

    /* Parse command-line arguments */
    if (argc != 2)
    {
        fprintf(stderr, "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -r <time slice> | -p ]\n"
            "    Default : FIFO Scheduler\n"
            "         -r : Round-Robin Scheduler\n"
            "         -p : Static Priority Scheduler\n\n");
        return -1;
    }
    cpu_count = atoi(argv[1]);

    /* FIX ME - Add support for -r and -p parameters*/


	head = NULL;
    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t*) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);


	pthread_mutex_init(&ready_mutex, NULL);
	pthread_cond_init(&is_empty,NULL);
    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}


