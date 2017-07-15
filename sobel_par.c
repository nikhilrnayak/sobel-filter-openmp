#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

//define start
#define COL 3840
#define ROW 2160
//define end


// global section start
int img_mat[ROW][COL];
int img_mat_filtered[ROW][COL];
int sobel_x[3][3] = {	{ -1, 0, 1 },
						{ -2, 0, 2 },
						{ -1, 0, 1 }
					};
int sobel_y[3][3] = {
						{ -1, -2, -1 },
						{  0,  0,  0 },
						{  1,  2,  1 }
					};
// global section end

// function prototypes start
void img_acquire( FILE * );
void img_write( FILE * );
void sobel_filter();
// function prototypes end


void main(int argc, char **argv)
{
	int i, j;
	clock_t start, end;
	double time_taken;
	FILE *fp_in = fopen("./read.txt", "r");
	FILE *fp_out = fopen("./write.txt","w");
	img_acquire( fp_in );
	omp_set_num_threads(4);
	start = clock();
	sobel_filter();
	end = clock();
	time_taken = (double)(end - start)/CLOCKS_PER_SEC;
	img_write( fp_out );
	printf("Time taken is %lf\n", time_taken);
}

// function definitions

void img_acquire( FILE *fp )
{
	int val, i, j;
	char flag = 'F';
	for( i = 0; i < ROW; i++ )
	{
		if( flag == 'T' )
			break;
		for( j = 0; j < COL; j++ )
		{
			if( fscanf( fp, "%d", &val) == EOF )
			{
				break;
				flag = 'T';
			}
		}	
	}	
	fclose( fp );
}

void img_write( FILE *fp )
{
	int val, i, j;
	char flag = 'F';
	for( i = 0; i < ROW; i++ )
	{
		for( j = 0; j < COL; j++ )
		{
			fprintf( fp, "%d\t", img_mat_filtered[i][j]);
			//val = 0;
		}	
		fprintf( fp, "\n" );
	}
	fclose( fp );
}

void sobel_filter()
{
	int i,j;
	int gx, gy, hxy;
	//#pragma omp parallel private( i, j, gx, gy, hxy ) default(shared)
	//{
		#pragma omp parallel for default(shared) private( i, j, gx, gy, hxy )
		for( i = 1; i < ROW - 1; i++ )
		{	printf("%d\n", omp_get_thread_num());
			for( j = 1; j < COL - 1; j++ )
			{
				gx = img_mat[i-1][j-1]*sobel_x[0][0] + img_mat[i-1][j]*sobel_x[0][1] + img_mat[i-1][j+1]*sobel_x[0][2]+
					 img_mat[i][j-1]*sobel_x[1][0] + img_mat[i][j]*sobel_x[1][1] + img_mat[i][j+1]*sobel_x[1][2]+
					 img_mat[i+1][j-1]*sobel_x[2][0] + img_mat[i+1][j]*sobel_x[2][1] + img_mat[i+1][j+1]*sobel_x[2][2];
				gy = img_mat[i-1][j-1]*sobel_y[0][0] + img_mat[i-1][j]*sobel_y[0][1] + img_mat[i-1][j+1]*sobel_y[0][2]+
					 img_mat[i][j-1]*sobel_y[1][0] + img_mat[i][j]*sobel_y[1][1] + img_mat[i][j+1]*sobel_y[1][2]+
					 img_mat[i+1][j-1]*sobel_y[2][0] + img_mat[i+1][j]*sobel_y[2][1] + img_mat[i+1][j+1]*sobel_y[2][2];
				hxy = sqrt((( gx * gx ) + ( gy * gy )));
				img_mat_filtered[ i ][ j ] = hxy;
			}
			
		}
		
	//}
}