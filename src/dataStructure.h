/*
 * DataStructure.h
 *
 *  Created on: Feb 15, 2018
 *      Author: gb1609
 */

#ifndef SRC_DATASTRUCTURE_H_
#define SRC_DATASTRUCTURE_H_
#include <vector>

struct Point
{
		float xllCorner;
		float yllCorner;
};
class DataStructure
{
	private:
		int nCols;
		int nRows;
		float cellSize;
		Point leftCorner;
		float noData;
		vector<vector<float> > matrix;
		float maxValue;
	public:
		DataStructure()
		{
		}
		void setcell(int c, int r, float value)
		{
			matrix[r][c] = value;
		}
		void addRow(string line)
		{
			vector<float> temp = explode(line, ' ');
			matrix.push_back(temp);
		}

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

		vector<vector<float> > getMatrix() const
		{
			return matrix;
		}

		void setCellSize(float cSize)
		{
			cellSize = cSize * 0.3;
		}

		void setLeftCorner(float x, float y)
		{
			leftCorner.xllCorner = x;
			leftCorner.yllCorner = y;
		}

		void setCols(int cols)
		{
			nCols = cols;
		}

		void setNoData(float nData)
		{
			noData = nData;
		}

		void setRows(int rows)
		{
			nRows = rows;
		}
		void printStructure()
		{
			cout << "col:" << nCols << "   row:" << nRows << endl;
			cout << "xC:" << leftCorner.xllCorner << "   yC:" << leftCorner.yllCorner << endl;
			cout << "cel:" << cellSize << "   noData:" << noData << endl;
		}
		void printMatrix()
		{
			for (int i = 0; i < nRows; i++)
			{
				for (int j = 0; j < nCols; j++)
					cout << "[ " << matrix[i][j] << " ]";
				cout << endl;
			}
		}
		const vector<float> explode(const string& s, const char& c)
		{
			string buff = "";
			vector<float> v;

			for (int i = 0; i < s.length(); i++)
			{
				if (s[i] != c)
					buff += s[i];
				else if (s[i] == c && buff != "")
				{
					v.push_back(strtof((buff).c_str(), 0));
					buff = "";
				}
			}
			if (buff != "")
				v.push_back(strtof((buff).c_str(), 0));

			return v;
		}
//		void generatevertex1step(float vertex[], int cellx, int celly, int & cont)
//		{
//			if (matrix[cellx][celly] == noData)
//			{
//				for (int i = 0; i < 36; i++)
//					vertex[cont++] = 0.0f;
//				return;
//			}
//			float valueCell = matrix[cellx][celly];
//			valueCell = 0.0f;
//			float maxV = getMax();
//			float vC = matrix[cellx][celly] / maxV;
//			float verticeX = beginx(cellx);
//			float verticeY = beginy(celly);
//			float temp[] =
//			{ verticeX, valueCell, verticeY, vC, vC, vC, verticeX + cellSize, valueCell, verticeY, vC, vC, vC, verticeX
//					+ cellSize, valueCell, verticeY + cellSize, vC, vC, vC, verticeX + cellSize, valueCell, verticeY
//					+ cellSize, vC, vC, vC, verticeX, valueCell, verticeY + cellSize, vC, vC, vC, verticeX, valueCell,
//					verticeY, vC, vC, vC };
//			for (int i = 0; i < 36; i++)
//			{
//				vertex[cont++] = temp[i];
//			}
//
//		}
		void generatevertex1step(vector<float>& vertex, vector<vector<float> > &vertexFloor, int cellx, int celly)
		{
			if (matrix[cellx][celly] == noData)
				return;
			float valueCell = matrix[cellx][celly];
			valueCell = 0.0f;
			float maxV = getMax();
			float vC = matrix[cellx][celly] / maxV;
			float verticeX = beginx(cellx);
			float verticeY = beginy(celly);
			float temp[] =
			{ verticeX, valueCell, verticeY, vC, vC, vC, verticeX + cellSize, valueCell, verticeY, vC, vC, vC, verticeX
					+ cellSize, valueCell, verticeY + cellSize, vC, vC, vC, verticeX + cellSize, valueCell, verticeY
					+ cellSize, vC, vC, vC, verticeX, valueCell, verticeY + cellSize, vC, vC, vC, verticeX, valueCell,
					verticeY, vC, vC, vC };
			for (int i = 0; i < 36; i++)
				vertex.push_back(temp[i]);


		}

		const int numberElementRow()
		{
			return 36 * nCols;
		}
		const int beginx(int cellx)
		{
			return cellx * (cellSize);
		}
		const int beginy(int celly)
		{
			return celly * (cellSize);
		}
		void printVertex(float vertex[])
		{
			int cont = 0;
			for (int i = 0; i < 36 * nCols * nRows; i++)
			{
				if (cont++ == 36)
				{
					cout << vertex[i] << endl;
					cont = 0;
				}
				else
					cout << vertex[i] << " ";
			}
		}
		float getMax()
		{
			int max = matrix[0][0];
			for (int i = 0; i < nRows; i++)
			{
				for (int j = 0; j < nCols; j++)
					if (matrix[i][j] > max)
						max = matrix[i][j];
			}
			return max;
		}
};

#endif /* SRC_DATASTRUCTURE_H_ */
