/*
* Copyright (c) 2018 ALSENET SA
*
* Author(s):
*
*      Luc Deschenaux <luc.deschenaux@freesurf.ch>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <sstream>
#include <fstream>
#include <limits>
#include <cstring>
#include "tinyply.h"
#include "ply_io.h"


typedef struct { double x; double y; double z; } double3;

int main(int argc, char **argv){
  int verbose=1;
  tinyply::PlyFile file;
  std::vector<RequestedProperties> requestList;
  std::shared_ptr<tinyply::PlyData> vertices=0;
  std::shared_ptr<tinyply::PlyData> colors=0;
  requestList.push_back(RequestedProperties(&vertices,"vertex",{"x","y","z"}));
  requestList.push_back(RequestedProperties(&colors,"vertex",{"red","green","blue"}));

  std::vector<std::shared_ptr<tinyply::PlyData>> otherProperties;
  ply_read((const char*)"test.ply", file, requestList, otherProperties, verbose);

  if (!vertices) {
    std::cerr << "failed to extract vertice properties from test.ply" << std::endl;
    return 1;
  }

  if (vertices->t==tinyply::Type::FLOAT64) {
    double3 *points=(double3*)vertices->buffer.get();
    for (size_t i=0; i<vertices->count ; ++i) {
      points[i].x=-points[i].x;
    }

  } else {
    std::cerr << "double or die" << std::endl;
    return 1;
  }

  std::cout << std::setprecision(std::numeric_limits<double>::digits10 + 1);
  file.write(std::cout,false);

}
