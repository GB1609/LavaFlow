/*
 * DataStructure.h
 *
 *  Created on: Feb 15, 2018
 *      Author: gb1609
 */

#ifndef SRC_DATASTRUCTURE_H_
#define SRC_DATASTRUCTURE_H_
struct Point
{
		float xllCorner;
		float yllCorner;
		Point(float a, float b)
		{
			xllCorner = a, yllCorner = b;
		}
};
class DataStructure
{
	private:
		int nCols;
		int nRows;
		float cellSize;
		Point leftCorner;
		float noData;
		float** matrix;
	public:
		DataStructure(int col, int row, float cSize, Point lCorner, float noD)
		{
			nCols = col;
			nRows = row;
			cellSize = cSize;
			leftCorner.xllCorner = lCorner.xllCorner;
			leftCorner.yllCorner = lCorner.yllCorner;
			noData = noD;
			matrix = new float[col][row];
		}

		void setcell(int c, int r, float value)
		{
			matrix[c][r] = value;
		}
		;
		virtual ~DataStructure();

		float getCellSize() const
		{
			return cellSize;
		}

		Point getLeftCorner() const
		{
			return leftCorner;
		}

		int getCols() const
		{
			return nCols;
		}

		float getNoData() const
		{
			return noData;
		}

		int getRows() const
		{
			return nRows;
		}

		float** getMatrix() const
		{
			return matrix;
		}
};

#endif /* SRC_DATASTRUCTURE_H_ */
