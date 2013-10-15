#include "InputPath.h"
#include "ScoreComponentCollection.h"
#include "TargetPhraseCollection.h"
#include "StaticData.h"
#include "TypeDef.h"
#include "AlignmentInfo.h"
#include "util/check.hh"

using namespace std;

namespace Moses
{
InputPath::
InputPath(const Phrase &phrase, const NonTerminalSet &sourceNonTerms,
          const WordsRange &range, const InputPath *prefixPath,
          const ScorePair *inputScore)
  :m_prefixPath(prefixPath)
  ,m_phrase(phrase)
  ,m_range(range)
  ,m_inputScore(inputScore)
  ,m_sourceNonTerms(sourceNonTerms)
  ,m_nextNode(1)
{
  //cerr << "phrase=" << phrase << " m_inputScore=" << *m_inputScore << endl;

}

InputPath::~InputPath()
{
  delete m_inputScore;
  RemoveAllInMap(m_targetPhrases);
}

const TargetPhraseCollection *InputPath::GetTargetPhrases(const PhraseDictionary &phraseDictionary) const
{
  std::map<const PhraseDictionary*, ActiveChartItem*>::const_iterator iter;
  iter = m_targetPhrases.find(&phraseDictionary);
  if (iter == m_targetPhrases.end()) {
    return NULL;
  }
  const ActiveChartItem &item = *iter->second;
  return item.GetTargetPhraseCollection();
}

const void *InputPath::GetPtNode(const PhraseDictionary &phraseDictionary) const
{
  std::map<const PhraseDictionary*, ActiveChartItem*>::const_iterator iter;
  iter = m_targetPhrases.find(&phraseDictionary);
  if (iter == m_targetPhrases.end()) {
    return NULL;
  }
  const ActiveChartItem &item = *iter->second;
  return item.GetPtNode();
}

void InputPath::SetTargetPhrases(const PhraseDictionary &phraseDictionary
                                 , const TargetPhraseCollection *targetPhrases
                                 , const void *ptNode)
{
  ActiveChartItem *value = new ActiveChartItem(targetPhrases, ptNode);
  m_targetPhrases[&phraseDictionary] = value;
}

void InputPath::SetTargetPhrasesChart(const PhraseDictionary &phraseDictionary
                                 , const TargetPhraseCollection *targetPhrases
                                 , const void *ptNode) const
{
  ActiveChartItem value(targetPhrases, ptNode);
  m_activeChart[&phraseDictionary].push_back(value);
}

const ActiveChart *InputPath::GetActiveChart(const PhraseDictionary &phraseDictionary) const
{
  std::map<const PhraseDictionary*, std::vector<ActiveChartItem>  >::const_iterator iter;
  iter = m_activeChart.find(&phraseDictionary);
  if (iter == m_activeChart.end()) {
	  return NULL;
  }

  return &iter->second;

}


const Word &InputPath::GetLastWord() const
{
  size_t len = m_phrase.GetSize();
  CHECK(len);
  const Word &ret = m_phrase.GetWord(len - 1);
  return ret;
}

std::ostream& operator<<(std::ostream& out, const InputPath& obj)
{
  out << &obj << " " << obj.GetWordsRange() << " " << obj.GetPrefixPath() << " " << obj.GetPhrase();

  out << "pt: ";
  std::map<const PhraseDictionary*, ActiveChartItem*>::const_iterator iter;
  for (iter = obj.m_targetPhrases.begin(); iter != obj.m_targetPhrases.end(); ++iter) {
    const PhraseDictionary *pt = iter->first;
    out << pt << " ";
  }

  return out;
}

}
