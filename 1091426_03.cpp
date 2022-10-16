#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include<iostream>
#include<ctime>
#include<cstdlib>
#include <unistd.h>
#include<vector>
#include<algorithm>
using namespace std;


pthread_mutex_t mutex;
bool battery = false;
bool aircraft = false;
bool propeller = false;
int sum = 0;
struct producer_data {
	unsigned long thread_id;
	bool pro_battery = false;
	bool pro_aircraft = false;
	bool pro_propeller = false;
	int pro_sum = 0;
};

struct dispatcher_data {
	int random;
	int dis_battery_sum = 0;
	int dis_aircraft_sum = 0;
	int dis_propeller_sum = 0;
};
struct last {
	int num;
	int count;
};
void* Producer(void* ptr) {
	producer_data* data;
	data = (producer_data*)ptr;
	while (sum < 50) {
		if (sum < 50)
			if (data->thread_id == 1) {
				pthread_mutex_lock(&mutex);
				if (data->pro_battery == false) {
					if (battery == true) {
						data->pro_battery = true;
						battery = false;
						cout << "Producer 1 (aircraft): get battery\n";
					}
				}
				if (data->pro_propeller == false) {
					if (propeller == true) {
						data->pro_propeller = true;
						propeller = false;
						cout << "Producer 1 (aircraft): get propeller\n";
					}
				}
				if (data->pro_battery == true && data->pro_propeller == true) {
					cout << "Producer 1(aircraft): OK, " << data->pro_sum + 1 << " drone(s)\n";
					sum++;
					data->pro_battery = false;
					data->pro_propeller = false;
					data->pro_sum++;
				}
				pthread_mutex_unlock(&mutex);
			}
			else {
				pthread_mutex_lock(&mutex);
				if (data->pro_battery == false) {
					if (battery == true) {
						data->pro_battery = true;
						battery = false;
						cout << "Producer " << data->thread_id << " : get battery\n";
					}
				}
				if (data->pro_aircraft == false) {
					if (aircraft == true) {
						data->pro_aircraft = true;
						aircraft = false;
						cout << "Producer " << data->thread_id << " : get aircraft\n";
					}
				}
				if (data->pro_propeller == false) {
					if (propeller == true) {
						data->pro_propeller = true;
						propeller = false;
						cout << "Producer " << data->thread_id << " : get propeller\n";
					}
				}
				if (data->pro_aircraft == true && data->pro_battery == true && data->pro_propeller == true) {
					cout << "Producer " << data->thread_id << ": OK, " << data->pro_sum + 1 << " drone(s)\n";
					sum++;
					data->pro_battery = false;
					data->pro_aircraft = false;
					data->pro_propeller = false;
					data->pro_sum++;
				}
				pthread_mutex_unlock(&mutex);
			}
	}
	return NULL;
}

void* Dispatcher(void* ptr) {
	while (sum < 50) {
		dispatcher_data* data;
		data = (dispatcher_data*)ptr;
		int random = rand() % 3 + 1;
		if (sum < 50) {
			pthread_mutex_lock(&mutex);
			switch (random) {
			case 1:
				if (battery == false) {
					cout << "Dispatcher : battery\n";
					battery = true;
					data->dis_battery_sum++;
				}
				break;
			case 2:
				if (aircraft == false) {
					cout << "Dispatcher : aircraft\n";
					aircraft = true;
					data->dis_aircraft_sum++;
				}
				break;
			case 3:
				if (propeller == false) {
					cout << "Dispatcher : propeller\n";
					propeller = true;
					data->dis_propeller_sum++;
				}
				break;
			default:
				break;
			}
			pthread_mutex_unlock(&mutex);
		}
		sleep(1);
	}
	return NULL;
}



int main(int argc, char* argv[]) {
	int mode = atoi(argv[1]);
	srand(atoi(argv[2]));
	int n2 = atoi(argv[2]);

	int rc;

	if (mode != 0 && mode != 1) {
		cout << "invalid value\n";
		return 0;
	}
	if (n2 > 100 || n2 < 0) {
		cout << "invalid value\n";
		return 0;
	}
	if (argv[3]) {
		cout << "invalid value\n";
		return 0;
	}
	if (mode == 0) {
		pthread_t pro_handles[3];
		producer_data pro_array[3];
		pthread_t dis_handle;
		dispatcher_data dis_data;
		/* 分流 thread */
		for (int i = 0; i < 3; i++) {
			pro_array[i].thread_id = i + 1;
			//cout << "123";
			rc = pthread_create(&pro_handles[i], NULL, &Producer, (void*)&pro_array[i]);
			if (rc) {
				printf("ERORR; return code from pthread_create() is %d\n", rc);
				exit(EXIT_FAILURE);
			}
		}
		rc = pthread_create(&dis_handle, NULL, &Dispatcher, (void*)&dis_data);
		if (rc) {
			printf("ERORR; return code from pthread_create() is %d\n", rc);
			exit(EXIT_FAILURE);
		}
		/* 合流 thread */
		for (int i = 0; i < 3; i++)
			pthread_join(pro_handles[i], NULL);
		pthread_join(dis_handle, NULL);

		pthread_mutex_destroy(&mutex);
		cout << "Total battery has been made:" << dis_data.dis_battery_sum << endl;
		cout << "Total aircraft has been made:" << dis_data.dis_aircraft_sum << endl;
		cout << "Total propeller has been made:" << dis_data.dis_propeller_sum << endl;
		last A[3];
		for (int i = 0; i < 3; i++) {
			A[i].num = pro_array[i].thread_id;
			A[i].count = pro_array[i].pro_sum;
		}

		last tmp;
		for (int i = 2; i > 0; i--)
		{
			for (int j = 0; j <= i - 1; j++)
			{
				if (A[j].count < A[j + 1].count)
				{
					tmp = A[j];
					A[j] = A[j + 1];
					A[j + 1] = tmp;
				}
			}
		}
		for (int i = 0; i < 3; i++) {
			if (A[i].num == 0) {
				cout << "Producer 1(aircraft): " << A[i].count << " drone(s)\n";
			}
			else {
				cout << "Producer " << A[i].num << ": " << A[i].count << " drone(s)\n";
			}
		}
	}
	return 0;
}