// //================================================\\
// ||Triple modular redundancy Matrix Multiplication ||
// |]================================================[|
// ||       	   Mir Erfan Gheibi    		     ||
// \\================================================//

//--------------------------- <Include> ---------------------------
#include <iostream> // I/O Operations
#include <cstdlib> // General Purpose Functions Like RNGs
#include <vector> // Vector STL Library
#include <fstream> // File Operations
#include <ctime> // Time Operations
#include <string> // String Operations
#include <thread> // C11 Thread Library
#include <mutex> // Race Condition Avoidance
#include <bitset> // Binary Representations
//--------------------------- </Include> --------------------------
//----------------- <Pre-defined Compiler Macros> -----------------

#if defined(_WIN64) // If the OS is Microsoft Windows

// Press any key to continue ...
void pause(){ 
	system("PAUSE");
}

void screen_wipe(){
	system("CLS");
}

#elif defined(__linux__) // If the OS is Linux

#include <pthread.h>
#include <sched.h>
#include <errno.h>

// Press enter to continue ...
void pause(){ 
	std::cout << "Press enter to continue ..."; 
    	std::cin.get();
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
}

void screen_wipe(){
	system("clear");
}

// Data Container for Multiplication on Independent Threads on Specific CPU Cores
struct mult_data{ 
	std::vector<std::vector<double> > first;
	std::vector<std::vector<double> > second;
	std::vector<std::vector<double> > result;
	int core_offset;
};
#endif
//----------------- </Pre-defined Compiler Macros> ----------------

//---------------------------- <Define> ---------------------------
#define MAX_MAT_DIM 10
//---------------------------- </Define> --------------------------

//----------------------- <Global Variables> ----------------------
std::mutex iomtx; // Mutex for iostream in threads
//----------------------- </Global Variables> ---------------------

//----------------------- <Function Prototypes> -------------------
void visual_header();
void visual_instructions();
void hardware_conc();
unsigned int validated_input_unsigned_int(unsigned int lower_bound, unsigned int upper_bound);
int validated_input_int(int lower_bound, int upper_bound);
double validated_input_double();
bool matrix_input(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b);
bool fill_random(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b);
void show_matrix(std::vector<std::vector<double> > input, char aorb, bool do_pause);
#if defined(_WIN64)
bool multiply(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b,std::vector<std::vector<double> > & c);
#elif defined(__linux__)
void *multiply(void *thread_args);
#endif
bool equality(std::vector<std::vector<double> > & a, std::vector<std::vector<double> > & b);
bool voter(std::vector<std::vector<double> > & a, std::vector<std::vector<double> > & b, std::vector<std::vector<double> > & c, std::vector<std::vector<double> > & res);
bool TMR(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b,std::vector<std::vector<double> > & c1, std::vector<std::vector<double> > & c2, std::vector<std::vector<double> > & c3,std::vector<std::vector<double> > & final_res, bool faulty , unsigned int fault_mask);
void single_rand_fault_injection(std::vector<std::vector<double> > & a);
bool fault_simulation(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b,std::vector<std::vector<double> > & c1, std::vector<std::vector<double> > & c2, std::vector<std::vector<double> > & c3,std::vector<std::vector<double> > & final_res);
bool dump_matrices(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b,std::vector<std::vector<double> > & c);
//---------------------- </Function Prototypes> -------------------
//----------------------- <Visual Functions> ----------------------
//------------------------ <Visual Header> ------------------------

// Displays a header containing some information
void visual_header(){ 
		screen_wipe();//system("CLS");
		std::cout<<"+------------------------------------------------+"<<std::endl;
		std::cout<<"| Triple Module Redundancy Matrix Multiplication |"<<std::endl;
		std::cout<<"| \tCourse Name:  Fault Tolerant Systems\t |"<<std::endl;
		std::cout<<"| \t     Midterm Exam | Fall 2018\t\t |"<<std::endl;
		std::cout<<"|   Mir Erfan Gheibi | Student Number: 96201532  |"<<std::endl;
		std::cout<<"+------------------------------------------------+"<<std::endl;
}
//------------------------ </Visual Header> -----------------------
//----------------------- <User Interface> ------------------------

// Displays menu and how to use program
void visual_instructions(){
	std::cout<<"| Enter a number to continue:\t\t\t |"<<std::endl;
	std::cout<<"| 0: Exit\t\t\t\t\t |"<<std::endl;
	std::cout<<"| 1: Hardware Concurrency Info. of Machine\t |"<<std::endl;
	std::cout<<"| 2: Input Matrices\t\t\t\t |"<<std::endl;
	std::cout<<"| 3: Generate Random Matrices with\t\t |\n|    Pre-Defined Maximum Dimension ("<<MAX_MAT_DIM<<"X"<<MAX_MAT_DIM<<")\t |"<<std::endl;
	std::cout<<"| 4: Show First Matrix\t\t\t\t |"<<std::endl;
	std::cout<<"| 5: Show Second Matrix\t\t\t\t |"<<std::endl;
	std::cout<<"| 6: TMR Matrix Multiplication\t\t\t |"<<std::endl;
	std::cout<<"| 7: Show Multiplication Result\t\t\t |"<<std::endl;
	std::cout<<"| 8: Fault Simulation (Doesn't affect the result)|"<<std::endl;
	std::cout<<"| 9: Dump Matrices to File\t\t\t |"<<std::endl;	
	std::cout<<"+------------------------------------------------+"<<std::endl;
}
//----------------------- </User Interface> -----------------------
//----------------------- </Visual Functions> ---------------------
//----------------------- <User Interactions> ---------------------
//-------------------- <User Input Validation> --------------------

// Validates unsigned int input and prevents crashes and wrong answers
unsigned int validated_input_unsigned_int(unsigned int lower_bound, unsigned int upper_bound){
	
	unsigned int input;
	while (true)
		{
			std::cout<<"Please enter an unsigned integer in range ["<<lower_bound<<","<<upper_bound<<"]:"<<std::endl;
			std::cout<<">";// Display Input Cursor
			std::cin>>input;
			// If the user inputs wrong input type, flush the stream and retry
			if(std::cin.fail()){// If error occurred
				std::cin.clear(); // Reset the error flags
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n'); //Extracts characters from the input sequence and discards them, until either n characters have been extracted, or one compares equal to delim.
				std::cout<<"Wrong input type, please follow the instructions."<<std::endl;
			}
			// If the user inputs right data type following by invalid characters
			else if(!std::cin.fail())
				// If any invalid character is in the input stream, flush the stream and retry
				if (std::cin.peek()!='\n'){ // If next character is not \n
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
					std::cout<<"Wrong input type, please follow the instructions."<<std::endl;
					continue;
				}
				else if(input<=upper_bound&&input>=lower_bound)
					break;
				else
					std::cout<<"Invalid range, please follow the instructions."<<std::endl;
		}
	return input;
}
// Validates integer input and prevents crashes and wrong answers
int validated_input_int(int lower_bound, int upper_bound){
	int input;
	while (true)
		{
			std::cout<<"Please enter an integer in range ["<<lower_bound<<","<<upper_bound<<"]:"<<std::endl;
			std::cout<<">";// Display Input Cursor
			std::cin>>input;
			// If the user inputs wrong input type, flush the stream and retry
			if(std::cin.fail()){// If error occurred
				std::cin.clear();// Reset the error flags
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n'); //Extracts characters from the input sequence and discards them, until either n characters have been extracted, or one compares equal to delim.
				std::cout<<"Wrong input type, please follow the instructions."<<std::endl;
			}
			// If the user inputs right data type following by invalid characters
			else if(!std::cin.fail())
				if (std::cin.peek()!='\n'){// If next character is not \n
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
					std::cout<<"Wrong input type, please follow the instructions."<<std::endl;
					continue;
				}
				else if(input<=upper_bound&&input>=lower_bound)
					break;
				else
					std::cout<<"Invalid range, please follow the instructions."<<std::endl;
		}
	return input;
}
// Validates double floating point input and prevents crashes and wrong answers
double validated_input_double(){
	double input;
	while (true)
		{
			std::cout<<"Please enter a double floating point:"<<std::endl;
			std::cout<<">";// Display Input Cursor
			std::cin>>input;
			// If the user inputs wrong input type, flush the stream and retry
			if(std::cin.fail()){// If error occurred
				std::cin.clear();// Reset the error flags
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');//Extracts characters from the input sequence and discards them, until either n characters have been extracted, or one compares equal to delim.
				std::cout<<"Wrong input type, please follow the instructions."<<std::endl;
			}
			// If the user inputs right data type following by invalid characters
			else if(!std::cin.fail()){
				if (std::cin.peek()!='\n'){// If next character is not \n
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
					std::cout<<"Wrong input type, please follow the instructions."<<std::endl;
					continue;
				}
				else
					break;
			}
		}
	return input;
}
//-------------------- </User Input Validation> --------------------
//---------------------- </User Interactions> ----------------------
//----------------------- <Matrix Operations> ----------------------

// Input two matrices (operands)
bool matrix_input(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b){
	int a_dim_r;
	int a_dim_c;
	int b_dim_r;
	int b_dim_c;

	std::cout<<"Notice: Dimensions of matrices intentionally limited to "<<MAX_MAT_DIM<<std::endl;
	while (true)
	{
		std::cout<<"Enter first matrix's number of rows"<<std::endl;
		a_dim_r=validated_input_int(1,MAX_MAT_DIM);
		std::cout<<"Enter first matrix's number of columns"<<std::endl;
		a_dim_c=validated_input_int(1,MAX_MAT_DIM);
		std::cout<<"Enter second matrix's number of rows"<<std::endl;
		b_dim_r=validated_input_int(1,MAX_MAT_DIM);
		std::cout<<"Enter second matrix's number of columns"<<std::endl;
		b_dim_c=validated_input_int(1,MAX_MAT_DIM);
		if (a_dim_c==b_dim_r)
			break;
		else{
			std::cout<<"Entered dimensions are invalid"<<std::endl;
			std::cout<<"Number of first matrix's columns should be equal to second matrix's rows"<<std::endl;
		}
	}
	a.resize(a_dim_r);
	b.resize(b_dim_r);
	std::cout<<"Input first matrix's values"<<std::endl;
	for(int i=0;i<a_dim_r;i++){
		a[i].resize(a_dim_c);
		for(int j=0;j<a_dim_c;j++){
			std::cout<<"Enter first matrix["<<i<<"]["<<j<<"]"<<std::endl;
			a[i][j]=validated_input_double();
		}
	}
	std::cout<<"Input second matrix's values"<<std::endl;
	for(int i=0;i<b_dim_r;i++){
		b[i].resize(b_dim_c);
		for(int j=0;j<b_dim_c;j++){
			std::cout<<"Enter first matrix["<<i<<"]["<<j<<"]"<<std::endl;
			b[i][j]=validated_input_double();
		}
	}
	
	return true;
}
// Generate two matrices (operands) with the Pre-Defined Maximum Dimension in range [0,99]
bool fill_random(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b){

	a.resize(MAX_MAT_DIM);
	b.resize(MAX_MAT_DIM);

	for(int i=0;i<MAX_MAT_DIM;i++){
		a[i].resize(MAX_MAT_DIM);
		for(int j=0;j<MAX_MAT_DIM;j++){
			a[i][j]=std::rand() % 100;
		}
	}
	for(int i=0;i<MAX_MAT_DIM;i++){
		b[i].resize(MAX_MAT_DIM);
		for(int j=0;j<MAX_MAT_DIM;j++){
			b[i][j]=std::rand() % 100;
		}
	}
	return true;
}
#if defined(_WIN64)
// Multiplies two matrices and stores in the third one
bool multiply(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b,std::vector<std::vector<double> > & c){
	int a_dim_r;
	int a_dim_c;
	int b_dim_r;
	int b_dim_c;

	a_dim_r=(int)a.size();
	a_dim_c=(int)a[0].size();
	b_dim_r=(int)b.size();
	b_dim_c=(int)b[0].size();

	c.resize(a_dim_r);
	for(int i=0;i<a_dim_r;i++){
		c[i].resize(b_dim_c);
		for(int j=0;j<b_dim_c;j++){
			c[i][j]=0;
		}}
			
	for(int i=0; i<a_dim_r;i++){
		for (int j = 0; j < b_dim_c; j++)
		{
			for (int k = 0; k < a_dim_c; k++)
			{
				c[i][j] += a[i][k]*b[k][j];
			}
		}
	}

	// Critical section (exclusive access to std::cout signaled by locking iomtx
	iomtx.lock();
	std::cout<<"Multiplication on thread number "<<std::this_thread::get_id()<<" : Done"<<std::endl; //Display thread Id
	iomtx.unlock();
	
	return true;
}
#elif defined(__linux__)
// Multiplies two matrices and stores in the argument struct
void *multiply(void *thread_args){
	
	struct mult_data *my_data;
	my_data = (struct mult_data *) thread_args; //Interpret arguments

	cpu_set_t cpuset; //The cpu_set_t data structure represents a set of CPUs.
    CPU_ZERO(&cpuset); //Clears set, so that it contains no CPUs.
    
	int ci=my_data->core_offset;
	CPU_SET(ci+1, &cpuset); //Add CPU cpu to set.

	pthread_t pth; 
	pth = pthread_self(); //The ID of the calling thread.

	int error = pthread_setaffinity_np(pth, sizeof(cpu_set_t), &cpuset);//Sets the CPU affinity mask of
																		//the thread to the CPU set pointed to by cpuset.  If the call
																		//is successful, and the thread is not currently running on one of the
																		//CPUs in cpuset, then it is migrated to one of those CPUs.
	if (error) { 
		std::cout<<"Affinity Set Error!"<<std::endl; 
		pthread_exit(NULL);//Terminates the calling thread
		exit(EXIT_FAILURE);
	} 

	std::vector<std::vector<double> > a = my_data->first;
	std::vector<std::vector<double> > b = my_data->second;
	std::vector<std::vector<double> > c;
	

	int a_dim_r;
	int a_dim_c;
	int b_dim_r;
	int b_dim_c;

	a_dim_r=(int)a.size();
	a_dim_c=(int)a[0].size();
	b_dim_r=(int)b.size();
	b_dim_c=(int)b[0].size();

	c.resize(a_dim_r);
	for(int i=0;i<a_dim_r;i++){
		c[i].resize(b_dim_c);
		for(int j=0;j<b_dim_c;j++){
			c[i][j]=0;
		}}
			
	for(int i=0; i<a_dim_r;i++){
		for (int j = 0; j < b_dim_c; j++)
		{
			for (int k = 0; k < a_dim_c; k++)
			{
				c[i][j] += a[i][k]*b[k][j];
			}
		}
	}
	my_data -> result = c;
	// Critical section (exclusive access to std::cout signaled by locking iomtx
	iomtx.lock();
	std::cout<<"Multiplication on CPU(core) number "<<sched_getcpu()<<" : Done"<<std::endl; //Display CPU(core) number
	iomtx.unlock();
	pthread_exit(NULL); //Terminates the calling thread
}
#endif
// Displays the matrices
void show_matrix(std::vector<std::vector<double> > input, char aorb, bool do_pause=true){
	if (aorb=='A') 
		std::cout<<"First Matrix : "<<std::endl;
	else if(aorb=='B')
		std::cout<<"Second Matrix : "<<std::endl;
	else
		std::cout<<"Multiplication Answer : "<<std::endl;

	int row_size=(int)input.size();
	int column_size=(int)input[0].size();
	
	for(int i=0;i<row_size;i++){
		std::cout<<"| ";
		for(int j=0; j<column_size;j++){
			std::cout<<input[i][j];
				std::cout<<"\t";
		}
		std::cout<<" |"<<std::endl;
	}
	std::cout<<std::endl;
	if(do_pause) // For fault simulation applications
		pause();
}
// Dumps matrices in files
bool dump_matrices(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b,std::vector<std::vector<double> > & c){
	int a_dim_r;
	int a_dim_c;
	int b_dim_r;
	int b_dim_c;
	int c_dim_r;
	int c_dim_c;

	std::ofstream ma,mb,mc;
	std::time_t now = time(0);
	std::string s_now = std::to_string(now);

	std::string matrix_a_file_name="first_matrix_" +s_now+".txt";
	std::string matrix_b_file_name="second_matrix_"+s_now+".txt";
	std::string matrix_c_file_name="res_matrix_" +s_now+".txt";

	ma.open(matrix_a_file_name,std::ios_base::app);

	if(!ma.is_open()){
		std::cout<<"File Creation Error!";
		return false;
	}
	mb.open(matrix_b_file_name,std::ios_base::app);
	if(!mb.is_open()){
		std::cout<<"File Creation Error!";
		return false;
	}
	mc.open(matrix_c_file_name,std::ios_base::app);
	if(!mc.is_open()){
		std::cout<<"File Creation Error!";
		return false;
	}

	a_dim_r = (int)a.size();
	a_dim_c = (int)a[0].size();
	b_dim_r = (int)b.size();
	b_dim_c = (int)b[0].size();
	c_dim_r = (int)c.size();
	c_dim_c = (int)c[0].size();

	for(int i=0;i<a_dim_r;i++){
		for(int j=0; j<a_dim_c;j++){
			ma<<a[i][j];
			if(j!=a_dim_c-1)
				ma<<"\t";
		}
		ma<<"\n";
	}

	for(int i=0;i<b_dim_r;i++){
		for(int j=0; j<b_dim_c;j++){
			mb<<b[i][j];
			if(j!=b_dim_c-1)
				mb<<"\t";
		}
		mb<<"\n";
	}

	for(int i=0;i<c_dim_r;i++){
		for(int j=0; j<c_dim_c;j++){
			mc<<c[i][j];
			if(j!=c_dim_c-1)
				mc<<"\t";
		}
		mc<<"\n";
	}

	ma.close();
	mb.close();
	mc.close();
	std::cout<<"Matrices are dumped successfully"<<std::endl;
	pause();
	return true;
}
//----------------------- </Matrix Operations> ---------------------
//-------------------------- <Miscellaneous> -----------------------

// Returns the number of hardware thread contexts
void hardware_conc(){
	if (std::thread::hardware_concurrency()==0){
		std::cout<< "The number of hardware thread contexts is not computable or well defined" <<std::endl;
	}
	else
		std::cout<<"The number of hardware thread contexts is "<<std::thread::hardware_concurrency()<<std::endl;
	pause();
	//system("PAUSE");
}
//------------------------- </Miscellaneous> -----------------------
//------------------------ <Fault Tolerancy> -----------------------

// Checks if the two input matrices are equal
bool equality(std::vector<std::vector<double> > & a, std::vector<std::vector<double> > & b){

	int a_dim_r;
	int a_dim_c;
	int b_dim_r;
	int b_dim_c;

	a_dim_r = (int)a.size();
	b_dim_r = (int)b.size();

	for(int i=0;i<a_dim_r;i++){
		a_dim_c = (int)a[i].size();
		b_dim_c = (int)b[i].size();

		if (a_dim_c!=b_dim_c)
			return false;

		for (int j = 0; j < a_dim_c; j++)
		{
			if(a[i][j]!=b[i][j]){
				return false;
			}
		}
	}

	return true;

}
// A majority voter for TMR
bool voter(std::vector<std::vector<double> > & a, std::vector<std::vector<double> > & b, std::vector<std::vector<double> > & c, std::vector<std::vector<double> > & res){

	if (equality(ref(a),ref(b))){
		res=a;
		return true;
	}
	else if (equality(ref(b),ref(c))){
		res=b;
		return true;
	}
	else if (equality(ref(a),ref(c))){
		res=a;
		return true;
	}
	else 
		return false;	
}
// Applies TMR
bool TMR(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b,std::vector<std::vector<double> > & c1, std::vector<std::vector<double> > & c2, std::vector<std::vector<double> > & c3,std::vector<std::vector<double> > & final_res, bool faulty = false, unsigned int fault_mask=0){

	#if defined(_WIN64)

	std::thread first (&multiply,a,b,ref(c1));
	std::thread second (&multiply,a,b,ref(c2));
	std::thread third (&multiply,a,b,ref(c3));
	first.join();
	second.join();
	third.join();
	
	#elif defined(__linux__)

	pthread_t threads[3];
	pthread_attr_t attr;

   	void *status;
	int rc;
   	pthread_attr_init(&attr); // Initializes the thread attributes with default attribute values.
   	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); // Sets the detach state attribute of the thread attributes to the specific value.
																// The detach state attribute determines whether a thread created using the thread attributes
																// will be created in a joinable or a detached state

	struct mult_data td[3];

	td[0].result=c1;
	td[1].result=c2;
	td[2].result=c3;

	for( int i = 0; i < 3; i++ ) {
      		std::cout <<"Creating thread, " << i << std::endl;

      		td[i].first = a;
      		td[i].second = b;
			td[i].core_offset=i;

      		rc = pthread_create(&threads[i], NULL, multiply, (void *)&td[i]);
      		if (rc) {
			std::cout << "Error:unable to create thread," << rc << std::endl;
         		exit(-1);
      			}
   	}
	pthread_attr_destroy(&attr);
	for( int i = 0; i < 3; i++ ) {
      		rc = pthread_join(threads[i], &status);
      		if (rc) {
         		std::cout << "Error:unable to join," << rc << std::endl;
         		exit(-1);
      			}
      
      		std::cout << "TMR: completed thread id :" << i ;
      		std::cout << "  exiting with status :" << status << std::endl;
   	}

	c1 = td[0].result;
	c2 = td[1].result;
	c3 = td[2].result;

	#endif
	// Fault simulation capability
	if(faulty){
	switch(fault_mask){
	case(0):
	break;
	case(1)://001
		single_rand_fault_injection(ref(c1));
	break;
	case(2)://010
		single_rand_fault_injection(ref(c2));
	break;
	case(3)://011
		single_rand_fault_injection(ref(c1));
		single_rand_fault_injection(ref(c2));
	break;
	case(4)://100
		single_rand_fault_injection(ref(c3));
	break;
	case(5)://101
		single_rand_fault_injection(ref(c1));
		single_rand_fault_injection(ref(c3));
	break;
	case(6)://110
		single_rand_fault_injection(ref(c2));
		single_rand_fault_injection(ref(c3));
	break;
	case(7)://111
		single_rand_fault_injection(ref(c1));
		single_rand_fault_injection(ref(c2));
		single_rand_fault_injection(ref(c3));
	break;
	default:
	break;
	}

	}	

	if (voter(ref(c1),ref(c2),ref(c3),ref(final_res))){
		show_matrix(final_res,'C',!faulty);
		return true;
	}
	else{
		std::cout<<"No Reliable Answer!"<<std::endl;
		if(!faulty)
			pause();
		//system("PAUSE");
		return false;
	}
}
//------------------------ </Fault Tolerancy> ----------------------
//------------------------ <Fault Simulation> ----------------------

// Injects a random fault in a random entry
void single_rand_fault_injection(std::vector<std::vector<double> > & a){

	int a_dim_r;
	int a_dim_c;
	a_dim_r = (int)a.size();
	a_dim_c = (int)a[0].size();

	int row = std::rand() % a_dim_r;
	int column = std::rand() % a_dim_c;

	a[row][column] = -1*(std::rand() % 100);
}
// Simulates 8 distinct combinations of faulty situations
bool fault_simulation(std::vector<std::vector<double> > & a,std::vector<std::vector<double> > & b,std::vector<std::vector<double> > & c1, std::vector<std::vector<double> > & c2, std::vector<std::vector<double> > & c3,std::vector<std::vector<double> > & final_res){
	std::cout<<"Fault Simulation Started: "<<std::endl;

	for(int i=0;i<8;i++){
		final_res.clear();
		std::cout<<"Injected Fault Mask : "<<std::bitset<3>(i)<<std::endl;
		TMR(ref(a),ref(b),ref(c1),ref(c2),ref(c3),ref(final_res),true,i);
	}
	pause();
	return true;
}
//------------------------ </Fault Simulation> ---------------------
int main(){

	unsigned int selector;	
	bool input_matrices=false;
	bool multiplication=false;

	std::srand ((unsigned int)time(NULL));

	std::vector<std::vector<double> > matrix_a;
	std::vector<std::vector<double> > matrix_b;
	std::vector<std::vector<double> > mult_answer_a;
	std::vector<std::vector<double> > mult_answer_b;
	std::vector<std::vector<double> > mult_answer_c;
	std::vector<std::vector<double> > final_mult_result;
	do
	{
		visual_header();
		visual_instructions();
		selector=validated_input_unsigned_int(0,9);
		//--------------------- <Program Flow Control> ---------------------
		switch (selector)
		{
		case(0):
			#if defined(__linux__)
			pthread_exit(NULL);
			#endif
			return EXIT_SUCCESS;
			break;
		case(1):
			hardware_conc();
			break;
		case(2):
			input_matrices=matrix_input(matrix_a,matrix_b);
			break;
		case(3):
			input_matrices=fill_random(matrix_a,matrix_b);
			break;
		case(4):
			if(!input_matrices){
				std::cout<<"Matrices are empty. First generate them."<<std::endl;
				pause();
				//system("PAUSE");
			}
			else
				show_matrix(matrix_a,'A');
			break;
		case(5):
			if(!input_matrices){
				std::cout<<"Matrices are empty. First generate them."<<std::endl;
				pause();
				//system("PAUSE");
			}
			else
				show_matrix(matrix_b,'B');
			break;
		case(6):
			multiplication=TMR(ref(matrix_a),ref(matrix_b),ref(mult_answer_a),ref(mult_answer_b),ref(mult_answer_c),ref(final_mult_result));
			break;
		case(7):
			if(!multiplication){
				std::cout<<"Multiplication result matrix is empty. \nFirst multiply matrices."<<std::endl;
				pause();
				//system("PAUSE");
			}
			else
				show_matrix(final_mult_result,'C');
			break;
		case(8):
			if(!input_matrices){
				std::cout<<"Matrices are empty. First generate them."<<std::endl;
				pause();
				//system("PAUSE");
			}
			else
				fault_simulation(ref(matrix_a),ref(matrix_b),ref(mult_answer_a),ref(mult_answer_b),ref(mult_answer_c),ref(final_mult_result));
			break;
		case(9):
			if(!input_matrices||!multiplication){
				std::cout<<"Matrices are empty or multiplication result matrix \nis empty. First generate and then multiply them"<<std::endl;
				pause();
				//system("PAUSE");
			}
			else
				dump_matrices(matrix_a,matrix_b,final_mult_result);
			break;
		default:
			break;
		}
		//--------------------- </Program Flow Control> ---------------------
	} while (true);
	#if defined(__linux__)
	pthread_exit(NULL);
	#endif
	return EXIT_SUCCESS;
}
