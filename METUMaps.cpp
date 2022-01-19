#include "METUMaps.h"
#include "GraphExceptions.h"

#include <iostream>

void METUMaps::PrintNotInJourney() const
{
    std::cout << "Device is not in a journey!" << std::endl;
}

void METUMaps::PrintUnableToChangeDestination() const
{
    std::cout << "Cannot change Destination during journey!" << std::endl;
}

void METUMaps::PrintUnableToChangeStartingLoc() const
{
    std::cout << "Cannot change Starting Location during journey!" << std::endl;
}

void METUMaps::PrintAlreadyInJourney() const
{
    std::cout << "Device is already in a journey!" << std::endl;
}

void METUMaps::PrintJourneyIsAlreadFinished() const
{
    std::cout << "Journey is already finished!" << std::endl;
}

void METUMaps::PrintLocationNotFound() const
{
    std::cout << "One (or both) of the locations are not found in the maps!" << std::endl;
}

void METUMaps::PrintJourneyCompleted() const
{
    std::cout << "Journey Completed!" << std::endl;
}

void METUMaps::PrintCachedLocationFound(const std::string &location0,
                                        const std::string &location1) const
{
    std::cout << "Route between \""
              << location0 << "\" and \""
              << location1 << "\" is in cache, using that..."
              << std::endl;
}

void METUMaps::PrintCalculatingRoutes(const std::string &location0,
                                      const std::string &location1) const
{
    std::cout << "Calculating Route(s) between \""
              << location0 << "\" and \""
              << location1 << "\"..."
              << std::endl;
}

std::string METUMaps::GenerateKey(const std::string &location0,
                                  const std::string &location1)
{
    return location0 + "/" + location1;
}

METUMaps::METUMaps(int potentialPathCount,
                   const std::string &mapFilePath)
{
    map = Graph(mapFilePath);
    this->potentialPathCount = potentialPathCount;
    cachedPaths = KeyedHashTable(map.TotalVertexCount() * potentialPathCount);
    this->inJourney = false;
}

void METUMaps::SetDestination(const std::string &name)
{
    if (inJourney == false)
        destination = name;
    else
        PrintUnableToChangeDestination();
}

void METUMaps::SetStartingLocation(const std::string &name)
{
    if (inJourney == false)
        startingLoc = name;
    else
        PrintUnableToChangeStartingLoc();
}

void METUMaps::StartJourney()
{
    if (inJourney == true)
    {
        PrintAlreadyInJourney();
        return;
    }

    try
    {
        std::vector<std::vector<int>> orderedVertexIdList;
        PrintCalculatingRoutes(startingLoc, destination);
        map.MultipleShortPaths(orderedVertexIdList, startingLoc, destination, potentialPathCount);
        inJourney = true;

        for (int index = 0; index < orderedVertexIdList.size(); index++)
        {
            std::vector<int> route = orderedVertexIdList[index];

            for (int k = 0; k < route.size() - 1; k++)
            {
                std::vector<int> sub_route;
                for (int temp_k = k; temp_k < route.size(); temp_k++)
                {
                    sub_route.push_back(route[temp_k]);
                }
                std::string location0 = map.VertexName(sub_route[0]);
                cachedPaths.Insert(GenerateKey(location0, destination), sub_route);
            }
        }
        currentLoc = startingLoc;
        currentRoute = orderedVertexIdList.front();
    }
    catch (...)
    {
        PrintLocationNotFound();
    }
}

void METUMaps::EndJourney()
{
    if (inJourney == false)
        PrintJourneyIsAlreadFinished();
    else
    {
        cachedPaths.ClearTable();
        inJourney = false;
        startingLoc = "";
        currentLoc = "";
        destination = "";
        currentRoute.clear();
    }
}

void METUMaps::UpdateLocation(const std::string &name)
{
    if (inJourney == false)
    {
        PrintNotInJourney();
        return;
    }

    try
    {
        if (name == destination)
        {
            PrintJourneyCompleted();
            currentLoc = name;
            return;
        }

        std::vector<int> temp_route;
        if (cachedPaths.Find(temp_route, GenerateKey(name, destination)))
        {
            PrintCachedLocationFound(name, destination);
            currentLoc = name;
            currentRoute = temp_route;
        }
        else
        {
            std::vector<std::vector<int>> orderedVertexIdList;
            PrintCalculatingRoutes(name, destination);
            map.MultipleShortPaths(orderedVertexIdList, name, destination, potentialPathCount);
            inJourney = true;

            for (int index = 0; index < orderedVertexIdList.size(); index++)
            {
                std::vector<int> route = orderedVertexIdList[index];

                for (int k = 0; k < route.size() - 1; k++)
                {
                    std::string initname = map.VertexName(orderedVertexIdList[index][k]);
                    std::vector<int> sub_route;
                    for (int temp_k = k; temp_k < route.size(); temp_k++)
                    {
                        sub_route.push_back(route[temp_k]);
                    }
                    cachedPaths.Insert(GenerateKey(initname, destination), sub_route);
                }
            }
            currentLoc = name;
            currentRoute = orderedVertexIdList.front();
        }
    }
    catch (...)
    {
        PrintLocationNotFound();
    }
}

void METUMaps::Display()
{
    if (!inJourney)
    {
        // We are not in journey, this function
        // shouldn't be called
        PrintNotInJourney();
        return;
    }

    int timeLeft = map.TotalWeightInBetween(currentRoute);

    // Print last known location
    std::cout << "Journey         : " << startingLoc << "->"
              << destination << "\n";
    std::cout << "Current Location: " << currentLoc << "\n";
    std::cout << "Time Left       : " << timeLeft << " minutes\n";
    std::cout << "Current Route   : ";
    map.PrintPath(currentRoute, true);

    std::cout.flush();
}