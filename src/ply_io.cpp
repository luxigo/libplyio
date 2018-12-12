/*
* Author(s):
*
*      Luc Deschenaux <luc.deschenaux@freesurf.ch>
*
* This software is in the public domain. Where that dedication is not
* recognized, you are granted a perpetual, irrevocable license to copy,
* distribute, and modify this file as you see fit. If these terms are
* not suitable to your organization, you may choose to license it
* under the terms of the 2-clause simplified BSD.
*
*/

#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <fstream>
#include "tinyply.h"
#include "ply_io.h"

RequestedProperties::RequestedProperties(std::shared_ptr<tinyply::PlyData> *tinyPlyDataPtr, std::string elementName, std::initializer_list<std::string>properties_list) {
  this->tinyPlyDataPtr=tinyPlyDataPtr;
  this->elementName=elementName;
  this->remaining=properties_list.size();
  for (auto prop : properties_list) {
    this->properties.push_back(std::pair<std::string,bool>(prop,false));
  }
}

RequestedProperties::RequestedProperties(std::shared_ptr<tinyply::PlyData> *tinyPlyDataPtr, std::string elementName, std::vector<std::string>properties_list) {
  this->tinyPlyDataPtr=tinyPlyDataPtr;
  this->elementName=elementName;
  this->remaining=properties_list.size();
  for (auto prop : properties_list) {
    this->properties.push_back(std::pair<std::string,bool>(prop,false));
  }
}

int ply_read(
  const char *filename,
  tinyply::PlyFile &file, // output file
  std::vector<RequestedProperties> &requestedPropertiesList,
  int verbose
) {
  std::vector<std::shared_ptr<tinyply::PlyData>> other_properties;
  return ply_read(filename, file, requestedPropertiesList, other_properties, verbose);
}

int ply_read(
  const char *filename,
  tinyply::PlyFile &file, // output file
  std::vector<RequestedProperties> &requestedPropertiesList,
  std::vector<std::shared_ptr<tinyply::PlyData>> &other_properties,
  int verbose
) {

  int total=0;
  std::ifstream ss(filename, std::ios::binary);
  if (verbose) std::cerr << "Opening " << filename << " ..." << std::endl;
  if (ss.fail()) throw std::runtime_error(std::string("failed to open ") + filename);

  file.parse_header(ss);

  for (auto const &e : file.get_elements()) {
    if (verbose) std::cerr << "element - " << e.name << " (" << e.size << ")" << std::endl;
    for (auto p : e.properties) {
      bool found=false;
      if (verbose) std::cerr << "\tproperty - " << p.name << " (" << tinyply::PropertyTable[p.propertyType].str << ")" << std::endl;
      for(auto &req : requestedPropertiesList) {
        if (*req.tinyPlyDataPtr) continue;
        if (e.name==std::string(req.elementName)) {
          for (auto &prop : req.properties) {
            if (p.name==std::string(prop.first)) {
              if (prop.second) throw std::runtime_error(std::string("property defined twice ") + prop.first);
              prop.second=true;
              found=true;
              --req.remaining;
              break;
            }
          }
          if (!req.remaining) {
            if (verbose) {
              std::cerr << "found ";
              int i=req.properties.size();
              for (auto const &prop : req.properties) {
                std::cerr << prop.first << ((--i)?", ":"");
              }
              std::cerr << std::endl;
            }

            std::vector<std::string> properties_list;
            for (auto const &prop : req.properties) {
              properties_list.push_back(std::string(prop.first));
            }

            *req.tinyPlyDataPtr=file.request_properties_from_element(e.name, properties_list);
            ++total;
          }
          if (found) break;
        }
      }
      if (!found) {
        other_properties.push_back(file.request_properties_from_element(e.name.c_str(), { p.name.c_str() }));
      }
    }
  }

  if (verbose) std::cerr << "Reading ply..." << std::endl;
  file.read(ss);

  return total;

}
