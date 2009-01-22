/***********************************************************************
** Matrix.h                                                           **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
**                                                                    **
** This program is free software; you can redistribute it and/or      **
** modify it under the terms of the GNU General Public License        **
** as published by the Free Software Foundation; either version 2     **
** of the License, or (at your option) any later version.             **
**                                                                    **
** This program is distributed in the hope that it will be useful,    **
** but WITHOUT ANY WARRANTY; without even the implied warranty of     **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      **
** GNU General Public License for more details.                       **
**                                                                    **
** You should have received a copy of the GNU General Public License  **
** along with this program; if not, write to the Free Software        **
** Foundation, Inc., 51 Franklin Street, Fifth Floor,                 **
** Boston, MA 02110-1301, USA.                                        **
***********************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class TransformationMatrix
{
	public:
		TransformationMatrix (const string &cmds);
		
	protected:
		void operator = (double *v);
		static TransformationMatrix& translation (TransformationMatrix &m, double tx, double ty);
		static TransformationMatrix& rotation (TransformationMatrix &m, double arc);
		static TransformationMatrix& scaling (TransformationMatrix &m, double sx, double sy);
		static TransformationMatrix& skewing (TransformationMatrix &m, double );
		
	private:
		double values[9];
};


template <typename T>
class Matrix
{
   public:
      Matrix (int r) : values(r), rows(r) {}
		T& operator () (int r, int c=0) const   {return values[rows*r+c];}
		Matrix& operator += (const Matrix &m);
		Matrix& operator -= (const Matrix &m);
		Matrix& operator *= (const Matrix &m);
		Matrix& operator *= (const T &c);
		Matrix& operator /= (const T &c);

   private:		
		vector<T> values;
		int rows;
};


template <typename T>
Matrix& Matrix<T>::operator += (const Matrix &m) {
	for (int i=0; i < values.size(); i++)
		values[i] += m.values[i];
	return *this;
}


template <typename T>
Matrix& Matrix<T>::operator -= (const Matrix &m) {
	for (int i=0; i < values.size(); i++)
		values[i] -= m.values[i];
	return *this;
}


template <typename T>
Matrix& Matrix<T>::operator *= (const T &c) {
	for (int i=0; i < values.size(); i++)
		values[i] *= c;
	return *this;
}


template <typename T>
Matrix& Matrix<T>::operator /= (const T &c) {
	for (int i=0; i < values.size(); i++)
		values[i] /= c;
	return *this;
}


template <typename T>
Matrix& Matrix<T>::operator *= (const Matrix &m) {
	Matrix<T> res(rows);
	for (int i=0; i < rows; i++)
		for (int j=0; j < rows; j++)
			for (int k=0; k < rows; k++)
				res(i,j) += (*this)(i,k) * m(k,j);
	return *this = res;
}


#endif
