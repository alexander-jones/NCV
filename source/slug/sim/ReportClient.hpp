/*
	namespace slug::sim
*/

template<typename T>
bool ReportClient::Report::permute(const std::vector<char>& data,
                                   std::vector<T>& result)
{
	result.resize(_permutation.size());
	const T* d = (const T*)data.data();
	for (unsigned int i = 0; i < _permutation.size(); ++i)
	{
		result[i] = d[_permutation[i]];
	}
	return true;
}
