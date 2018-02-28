/*
 * DataStructure.h
 *
 *  Created on: Feb 15, 2018
 *      Author: gb1609
 */

#ifndef SRC_DATASTRUCTURE_H_
#define SRC_DATASTRUCTURE_H_
#include <vector>
using namespace std;
struct Point
{
		float xllCorner;
		float yllCorner;
};
struct SingleVertex
{
		float x;
		float y;
		float altitude;
		float temperature;
		int posV;
		float maxValue;
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
		vector<vector<SingleVertex> > support;
	public:
		DataStructure() // @suppress("Class members should be properly initialized")
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

		int getCols() const
		{
			return nCols;
		}

		int getRows() const
		{
			return nRows;
		}

		void setCellSize(float cSize)
		{
			cellSize = cSize;
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

		void constructSUPPORT(DataStructure& lavaTemp)
		{

			for (int i = 0; i < nRows + 1; i++)
			{
				vector<SingleVertex> t;
				for (int j = 0; j < nCols + 1; j++)
				{
					SingleVertex s;
					t.push_back(s);
				}
				support.push_back(t);
			}

			//			////////////////VERTICI CENTRALI//////////////////
			//			for (int i = 0; i < nRows; i++)
			//				for (int j = 0; j < nCols; j++)
			//				{
			//					support[i][j].x = begin(i) + halfSize;
			//					support[i][j].y = begin(j) + halfSize;
			//					support[i][j].altitude = matrix[i][j];
			//				}
			//angolo in alto a sinistra matrice
			support[0][0].x = begin(0);
			support[0][0].y = 0.0f;
			support[0][0].altitude = matrix[0][0];
			support[0][0].temperature = lavaTemp.getCell(0, 0);

			//angolo in alto a destra
			support[0][nCols].x = 0.0f;
			support[0][nCols].y = begin(nCols);
			support[0][nCols].altitude = matrix[0][nCols - 1];
			support[0][nCols].temperature = lavaTemp.getCell(0, nCols - 1);

			//angolo in basso a destra
			support[nRows][nCols].x = begin(nRows);
			support[nRows][nCols].y = begin(nCols);
			support[nRows][nCols].altitude = matrix[nRows - 1][nCols - 1];
			support[nRows][nCols].temperature = lavaTemp.getCell(nRows - 1, nCols - 1);
			//angolo in basso a sinistra
			support[nRows][0].x = begin(nRows);
			support[nRows][0].y = begin(0);
			support[nRows][0].altitude = matrix[nRows - 1][0];
			support[nRows][0].temperature = lavaTemp.getCell(nRows - 1, 0);

			for (int i = 1; i < nRows; i++)
			{
				//prima colonna
				support[i][0].x = begin(i);
				support[i][0].y = 0.0f;
				support[i][0].altitude = excludeNoDataBorder(matrix[i - 1][0], matrix[i][0]);
				support[i][0].temperature = lavaTemp.excludeNoDataBorder(lavaTemp.getCell(i - 1, 0),
						lavaTemp.getCell(i, 0));

				//ultima ulitma colonna
				support[i][nCols].x = begin(i);
				support[i][nCols].y = begin(nCols);
				support[i][nCols].altitude = excludeNoDataBorder(matrix[i - 1][nCols - 1], matrix[i][nCols - 1]);
				support[i][nCols].temperature = lavaTemp.excludeNoDataBorder(lavaTemp.getCell(i - 1, nCols - 1),
						lavaTemp.getCell(i, nCols - 1));
			}

			for (int i = 1; i < nCols; i++)
			{
				//prima riga
				support[0][i].x = 0.0f;
				support[0][i].y = begin(i);
				support[0][i].altitude = excludeNoDataBorder(matrix[0][i - 1], matrix[0][i]);
				support[0][i].temperature = lavaTemp.excludeNoDataBorder(lavaTemp.getCell(0, i - 1),
						lavaTemp.getCell(0, i));

				//ultima riga
				support[nRows][i].x = begin(nRows);
				support[nRows][i].y = begin(i);
				support[nRows][i].altitude = excludeNoDataBorder(matrix[nRows - 1][i - 1], matrix[nRows - 1][i]);
				support[nRows][i].temperature = lavaTemp.excludeNoDataBorder(lavaTemp.getCell(nRows - 1, i - 1),
						lavaTemp.getCell(nRows - 1, i));
			}

			//vertici interni
			for (int i = 1; i < nRows; i++)
				for (int j = 1; j < nCols; j++)
				{
					support[i][j].x = begin(i);
					support[i][j].y = begin(j);
					support[i][j].altitude = excludeNoDataCenter(matrix[i - 1][j - 1], matrix[i - 1][j],
							matrix[i][j - 1], matrix[i][j]);
					support[i][j].temperature = lavaTemp.excludeNoDataCenter(lavaTemp.getCell(i - 1, j - 1),
							lavaTemp.getCell(i - 1, j), lavaTemp.getCell(i, j - 1), lavaTemp.getCell(i, j));
				}
		}

		void generateRowFirstStep(vector<float>& vertex, vector<float>& color, int cellX, int cellY)
		{
			if (matrix[cellX][cellY] == noData)
				return;
			float valueCell = matrix[cellX][cellY];
			float valueCellNextX = matrix[cellX + 1][cellY];
			float valueCellNextY = matrix[cellX][cellY + 1];
			float nextNext = matrix[cellX + 1][cellY + 1];
			float colorVertex = excludeNoDataCenter(valueCell, nextNext, valueCellNextX, valueCellNextY)
					* (1.0f / getMax());
			float verticeX = begin(cellX);
			float verticeY = begin(cellY);
			if (cellY == nCols - 1)
			{
				valueCellNextY = valueCell;
				nextNext = valueCellNextX;
			}
			if (cellX == nRows - 1)
			{
				valueCellNextX = valueCell;
				nextNext = valueCellNextY;
			}
			if (cellY == nCols - 1 && cellX == nRows - 1)
				nextNext = valueCell;

			float temp[] =
			{ verticeX, verticeY, valueCell, colorVertex, colorVertex, colorVertex, verticeX + cellSize, verticeY,
					valueCellNextX, colorVertex, colorVertex, colorVertex, verticeX + cellSize, verticeY + cellSize,
					nextNext, colorVertex, colorVertex, colorVertex, verticeX, verticeY + cellSize, valueCellNextY,
					colorVertex, colorVertex, colorVertex };
			int cont = 0;
			for (int i = 0; i < 24; i++)
			{
				if (cont++ < 3)
					vertex.push_back(temp[i]);
				else
					color.push_back(temp[i]);
				if (cont == 6)
					cont = 0;
			}

		}

		void generateIndexFirstStep(vector<unsigned int>& index)
		{
			index.resize((nCols - 1) * 6);
			int cont = 0;
			for (int j = 0; j < nCols - 1; j++)
			{
				int increment = j * 4;
				int a = increment;
				int b = increment + 1;
				int c = increment + 2;
				int d = increment + 3;

				index[cont++] = a;
				index[cont++] = b;
				index[cont++] = c;
				index[cont++] = c;
				index[cont++] = d;
				index[cont++] = a;

			}

		}

		float getCell(int i, int j)
		{
			if (matrix[i][j] == noData)
				return 0.0f;
			else
				return matrix[i][j];
		}
		void addThickness(DataStructure& ds)
		{
			if (ds.getCols() != nCols || ds.getRows() != nRows)
			{
				cout << "non è possibile aggiungere, dimensioni diverse" << endl;
				return;
			}
			for (int i = 0; i < nRows; i++)
				for (int j = 0; j < nCols; j++)
					if (ds.getCell(i, j) != ds.getNoData())
						matrix[i][j] += ds.getCell(i, j);
		}
		float excludeNoDataCenter(float a, float b, float c, float d)
		{
			float sum = 0.0f;
			int num = 0;
			if (a != noData)
			{
				sum += a;
				num++;
			}
			if (b != noData)
			{
				sum += b;
				num++;
			}
			if (c != noData)
			{
				sum += c;
				num++;
			}
			if (d != noData)
			{
				sum += d;
				num++;
			}
			if (num == 0)
				return 0.0f;
			return sum / num;
		}
		float excludeNoDataBorder(float a, float b)
		{
			float sum = 0.0f;
			int num = 0;
			if (a != noData)
			{
				sum += a;
				num++;
			}
			if (b != noData)
			{
				sum += b;
				num++;
			}
			if (num == 0)
				return 0.0f;
			return sum / num;
		}

		void constructAll(vector<float>&fVertex, vector<unsigned int>& index, vector<float>& normali,
				vector<float>& textures, DataStructure& lavaTemp, vector<float>& colorTemp)
		{
			fVertex.resize(3 * (nRows + 1) * (nCols + 1));
			colorTemp.resize(fVertex.size() / 3);

			///CREAZIONE VERTICI FINALI
			int contV = 0;
			int contT = 0;
			float maxTemp = lavaTemp.getMax();
			for (int i = 0; i < nRows + 1; i++)
				for (int j = 0; j < nCols + 1; j++)
				{
					support[i][j].posV = contV;
					colorTemp[contT++] = support[i][j].temperature * (1.0f / maxTemp);
					fVertex[contV++] = support[i][j].x;
					fVertex[contV++] = support[i][j].y;
					fVertex[contV++] = support[i][j].altitude;
				}
			int a = 0;
			index.resize(nCols * nRows * 6);
			for (int i = 0; i < nRows; i++)
				for (int j = 0; j < nCols; j++)
				{
					index[a++] = (i * (nCols + 1)) + j;
					index[a++] = ((i + 1) * (nCols + 1)) + j;
					index[a++] = (i * (nCols + 1)) + j + 1;
					index[a++] = ((i + 1) * (nCols + 1)) + j;
					index[a++] = ((i + 1) * (nCols + 1)) + j + 1;
					index[a++] = (i * (nCols + 1)) + j + 1;
				}

			//NORMALOI

			normali.resize(fVertex.size());
			textures.resize((nCols + 1) * (nRows + 1) * 2);	//per 2 perche le texture sono 2d, ho perso un ora perche facevo per 3. mannaaia a dio.

			float AX1, AY1, AZ1, AX2, AY2, AZ2, BX1, BX2, BY1, BY2, BZ1, BZ2, NX1, NX2, NY1, NY2, NZ1, NZ2;
			for (int i = 0; i < nRows; i++)
			{
				for (int j = 0; j < nCols; j++)
				{
					SingleVertex tL = support[i][j];
					SingleVertex tR = support[i][j + 1];
					SingleVertex bR = support[i + 1][j + 1];
					SingleVertex bL = support[i + 1][j];

					AX1 = tR.x - tL.x;
					AY1 = tR.y - tL.y;
					AZ1 = tR.altitude - tL.altitude;
					BX1 = bL.x - tL.x;
					BY1 = bL.y - tL.y;
					BZ1 = bL.altitude - tL.altitude;

					NX1 = -((AY1 * BY1) - (AZ1 * BY1));
					NY1 = -((AZ1 * BX1) - (AX1 * BZ1));
					NZ1 = -((AX1 * BY1) - (AY1 * BX1));

					normali[tL.posV] = NX1;
					normali[tL.posV + 1] = NY1;
					normali[tL.posV + 2] = NZ1;

					if (j + 1 == nCols)
					{
						normali[tR.posV] = NX1;
						normali[tR.posV + 1] = NY1;
						normali[tR.posV + 2] = NZ1;
					}
					if (i + 1 == nRows)
					{
						normali[bL.posV] = NX1;
						normali[bL.posV + 1] = NY1;
						normali[bL.posV + 2] = NZ1;
					}
					if (j + 1 == nCols && i + 1 == nRows)
					{
						normali[bR.posV] = NX1;
						normali[bR.posV + 1] = NY1;
						normali[bR.posV + 2] = NZ1;
					}

				}

			}
			int c = 0;
			for (int i = 0; i < nRows + 1; i++)
				for (int j = 0; j < nCols + 1; j++)
				{
					textures[c++] = (cellSize * j) / (cellSize * nCols);
					textures[c++] = ((cellSize * nRows) - (cellSize * i)) / (cellSize * nRows);
				}
		}
		void printText(vector<float>& text)
		{
			cout << "NUMERO DI COORDINATE: " << text.size() / 2 << endl;
			int a = 0;
			for (int i = 0; i < text.size(); i++)
				if (a++ == 1)
				{
					a = 0;
					cout << "[" << text[i] << "]" << endl;
				}
				else
					cout << "[" << text[i] << "]";
		}
		void printSupport(vector<vector<SingleVertex> >& support)
		{
			int a = 0;
			for (int i = 0; i < nRows + 1; i++)
				for (int j = 0; j < nCols + 1; j++)
				{
					if (a++ == nCols)
					{
						a = 0;
						cout << "[" << support[i][j].x << " ; " << support[i][j].y << " ; " << support[i][j].altitude
								<< "]" << endl;

					}
					else
					{
						cout << "[" << support[i][j].x << " ; " << support[i][j].y << " ; " << support[i][j].altitude
								<< "]";
					}
				}
		}

		void printIndex(vector<unsigned int>& finalIndex, vector<float>& fVertex)
		{
			cout << "NUMERO DI COORDINATE: " << finalIndex.size() / 3 << endl;
			int c = 0;
			for (int i = 0; i < finalIndex.size(); i++)
				if (c++ == 2)
				{
					cout << "[" << finalIndex[i] << "->" << fVertex[finalIndex[i] * 3] << ";"
							<< fVertex[(finalIndex[i] * 3) + 1] << ";" << fVertex[(finalIndex[i] * 3) + 2] << "]"
							<< endl;
					c = 0;
				}
				else
					cout << "[" << finalIndex[i] << "->" << fVertex[finalIndex[i] * 3] << ";"
							<< fVertex[(finalIndex[i] * 3) + 1] << ";" << fVertex[(finalIndex[i] * 3) + 2] << "]";
		}
		void printVertex(vector<float>& vertex)
		{
			cout << "NUMERO DI COORDINATE: " << vertex.size() / 3 << endl;
			int c = 0;
			for (int i = 0; i < vertex.size(); i++)
				if (c++ == 2)
				{
					cout << "[" << vertex[i] << "]" << endl;
					c = 0;
				}
				else
					cout << "[" << vertex[i] << "]";
		}
		int begin(int cell)
		{
			return cell * (cellSize);
		}
		float getMax()
		{
			return maxValue;
		}

		void setMax()
		{
			int max = matrix[0][0];
			for (int i = 0; i < nRows; i++)
			{
				for (int j = 0; j < nCols; j++)
					if (matrix[i][j] > max)
						max = matrix[i][j];
			}
			maxValue = max;
		}

		const vector<vector<float> >& getMatrix() const
		{
			return matrix;
		}

		float getNoData() const
		{
			return noData;
		}
};

#endif /* SRC_DATASTRUCTURE_H_ */
