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
			cellSize = cSize * 0.3f;
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
		void generateVertexAndIndex(vector<float>& vertex, vector<unsigned int>& index)
		{
			for (int i = 0; i < nRows; i++)
				for (int j = 0; j < nCols; j++)

				{
					if (matrix[i][j] != noData)
					{
						vertex.push_back(begin(j));
						vertex.push_back(begin(i));
						vertex.push_back(0.0f);
					}
				}
			for (int i = 0; i < nRows - 1; i++)
				for (int j = 0; j < nCols - 1; j++)
				{
					unsigned int topLeftI = (nCols * i) + j;
					unsigned int bottomLeftI = (nCols * (i + 1)) + j;
					unsigned int topRightI = topLeftI + 1;
					unsigned int bottomRight = bottomLeftI + 1;

					index.push_back(topRightI);
					index.push_back(bottomLeftI);
					index.push_back(topLeftI);

					index.push_back(bottomRight);
					index.push_back(bottomLeftI);
					index.push_back(topRightI);
				}

		}
		void generatevertex1step(vector<float>& vertex, int cellx, int celly)
		{
			if (matrix[cellx][celly] == noData)
				return;
			float valueCell = matrix[cellx][celly];
			float valueCellNextX = matrix[cellx + 1][celly];
			float valueCellNextY = matrix[cellx][celly + 1];
			float nextNext = matrix[cellx + 1][celly + 1];
			float vC = matrix[cellx][celly] / getMax();
			float verticeX = begin(cellx);
			float verticeY = begin(celly);
			if (celly == nCols - 1)
			{
				valueCellNextY = valueCell;
				nextNext = valueCellNextX;
			}
			if (cellx == nRows - 1)
			{
				valueCellNextX = valueCell;
				nextNext = valueCellNextY;
			}
			if (celly == nCols - 1 && cellx == nRows - 1)
				nextNext = valueCell;

			float temp[] =
			{ verticeX, valueCell, verticeY, vC, vC, vC, verticeX + cellSize, valueCellNextX, verticeY, vC, vC, vC,
					verticeX + cellSize, nextNext, verticeY + cellSize, vC, vC, vC, verticeX + cellSize, nextNext,
					verticeY + cellSize, vC, vC, vC, verticeX, valueCellNextY, verticeY + cellSize, vC, vC, vC,
					verticeX, valueCell, verticeY, vC, vC, vC };
			for (int i = 0; i < 36; i++)
				vertex.push_back(temp[i]);

		}

		const int numberElementRow()
		{
			return 36 * nCols;
		}
		const int begin(int cell)
		{
			return cell * (cellSize);
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
		void print(vector<float>& v, vector<unsigned int>& index)
		{
			cout << "VERTEX:" << endl;
			int cont = 0;
			for (int i = 0; i < v.size(); i++)
			{
				if (cont++ == 2)
				{
					cout << v[i] << endl;
					cont = 0;
				}
				else
					cout << v[i] << "  ";
			}
			cont = 0;
			cout << "INDEX:" << endl;
			for (int i = 0; i < index.size(); i++)
			{
				if (cont++ == 2)
				{
					cout << index[i] << endl;
					cont = 0;
				}
				else
					cout << index[i] << "  ";
			}

		}
		void printVertex(vector<vector<float> > vectorVertex)
		{
			for (int i = 0; i < vectorVertex.size(); i++)
			{
				int cont = 0;
				for (int j = 0; j < vectorVertex[i].size(); j++)
				{
					if (cont++ == 2)
					{
						cout << "[" << vectorVertex[i][j] << "]" << endl;
						cont = 0;
						j += 3;
					}
					else
						cout << "[" << vectorVertex[i][j] << "]";

				}
			}
		}
};

#endif /* SRC_DATASTRUCTURE_H_ */
