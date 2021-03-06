#ifndef CONSTRAINED_MARKOV_H
#define CONSTRAINED_MARKOV_H

#include <string>
#include <vector>
#include <unordered_map>
#include <random>

#include "markov.h"

using namespace std;


/**
 * @brief Constrained Markov Model
 */
class ConstrainedMarkovModel {
public:
  
  ConstrainedMarkovModel();
  ~ConstrainedMarkovModel() {};

  /**
   * @brief Train the constrained markov model using training sentences
   * 
   * Reads in the training text at the given filePath and increments
   * the transition probabilities while iterating over words.
   * 
   * Removes probabilities that don't satisfy the constraint, then
   * backpropagates through the matrices to remove probabilties that
   * end before the word length is satisfied
   * 
   * Finally normalizes the probabilities
   * 
   * @param model trained markov model to use
   * @param constraint for NHMM
   * @author Porter Glines 1/13/19
   */
  void train(MarkovModel model, vector<string> constraint);

  /**
   * @brief Generates a sentence
   * 
   * @return vector<string> array of words making up a sentence
   * @author Porter Glines 1/13/19
   */
  vector<string> generateSentence();

  /**
   * @brief Generates a sentence
   * 
   * @param options program options
   * @return vector<vector<string> > array of generated sentences
   * @author Porter Glines 2/26/20
   */
  vector<vector<string> > generateSentences(Options options);

  /**
   * @brief Get the probability of a specific sentence being generated
   * 
   * multiplies the probabilities between each word to get the total
   * probability of a sentence
   * 
   * @param sentence generated sentence
   * @return double probability of the given sentence
   */
  double getSentenceProbability(vector<string> sentence);

  /**
   * @brief Get the length the model has trained on
   * 
   * @return int 
   */
  int getSentenceLength() { return sentenceLength; }

  /**
   * @brief Print the transition probabilities for debugging
   */
  void printTransitionProbs();

  /**
   * @brief Get the sizes of the transition matrices for debugging
   * 
   * @return vector<int> sizes of matrices
   * @author Porter Glines 1/28/19
   */
  vector<int> getTransitionMatricesSizes();

  /**
   * @brief Get the Training Sequences object
   * 
   * @return vector< vector<string> > training sequences
   * @author Porter Glines 5/24/19
   */
  vector< vector<string> > getTrainingSequences();

  /**
   * @brief Get the Markov Order object
   * 
   * @return int markov order (lookahead) of model
   * @author Porter Glines 2/26/20
   */
  int getMarkovOrder() { return markovOrder; }

  /**
   * @brief Print debug information about the model
   * 
   * information includes
   * - Markov order
   * - training sentence count
   * - transition matrices sizes
   * 
   * @param options program options
   * @author Porter Glines 2/26/20
   */
  void printDebugInfo(Options options);

  /**
   * @brief Get the Total Solution Count of a trained model
   * 
   * This is a very time consuming depth first search of the model's
   * structure
   * 
   * @return int count of solutions
   * @author Porter Glines 2/26/20
   */
  int getTotalSolutionCount();

  /**
   * @brief 
   * 
   */
  string sampleRemovedNodeByConstraint(int layerIndex);

  /**
   * @brief 
   * 
   */
  string sampleRemovedNodeByArcConsistency(int layerIndex);


protected:
  /// Marker representing the start of a sentence
  const string START = "<<START>>";
  /// Marker representing the end of a sentence
  const string END = "<<END>>";

  /// Specifies the markov order (lookahead distance) for the model
  int markovOrder;

  int sentenceLength;


  /// Random generator
  mt19937 randGenerator;
  /// Random distribution used by the generator
  uniform_real_distribution<double> randDistribution;

  /// Transition probability matrices between words
  vector< unordered_map< string, unordered_map<string, double> > > transitionMatrices;

  vector< vector<string> > removedNodesbyConstraint;

private:
  /// Stores training sentences used to train the model
  vector< vector<string> > trainingSequences;

  /// Original transition probability matrices mapping words -> (word, prob), (word, prob)...
  unordered_map< string, unordered_map<string, double> > transitionProbs;

  ///
  vector< vector<string> > removedNodesbyArcConsistency;

  /**
   * @brief Apply constraints to the transition matrices
   * 
   * Modify (delete) nodes int transitionMatrices[] that violate 
   * the constraint rules.
   * 
   * This is a pure virtual function
   */
  virtual void applyConstraints(vector<string> constraint) = 0;  // TODO: make parameter generic

  /**
   * @brief Remove nodes that violate arc consistency
   * Should be called after applying constraints and
   * before normalizing
   * 
   * enforces arc-consistency
   * 
   * @author Porter Glines 1/21/19
   */
  void removeDeadNodes();

  /**
   * @brief Adds a transition layer from START to the next layer
   * should be called after all other layers are settled but not
   * before the transition matrices are normalized
   * 
   * @author Porter Glines 1/21/19
   */
  void addStartTransition();

  /**
   * @brief Get the next word in a sentence given the previous word
   * 
   * Adheres to the markov property
   * 
   * @param prevWord previous word
   * @param wordIndex 
   * @return string next word generated
   */
  string getNextWord(string prevWord, int wordIndex);

  /**
   * @brief Calculate the probability of a sentence
   * 
   * @param sentence sentence consisting of words
   * @return double probability of sentence
   * @author Porter Glines 1/26/19
   */
  double calculateProbability(vector<string> sentence);


  /**
   * @brief Calculate the frequencies of words
   * 
   * The frequencies can be used as the prior probabilities
   * 
   * @param sentences training sentences
   * @return unordered_map<string, int> Frequences map (word, frequency)
   * @author Porter Glines 1/26/19
   */
  unordered_map<string, int> getWordFrequencies(vector< vector<string> > sentences);

  /**
   * @brief Normalize the transitionMatrices according to the method
   * described by Pachet **CITE
   * 
   * The normalized transitionMatrices retains the same probability
   * distribution as the original transitionMatrices but will then
   * be stochastic (each row adding up to 1.0)
   * 
   * @author Porter Glines 1/22/19
   */
  void normalize();

  /**
   * @brief Increment the probability in the probability matrix for a word given its next word
   * 
   * @param transitionProbs transition probability matrix mapping words -> (word, prob), (word, prob)...
   * @param word current word
   * @param nextWord next word
   */
  void increment(unordered_map< string, unordered_map<string, double> > &transitionProbs, string word, string nextWord);

  /**
   * @brief 
   * 
   */
  string sampleRemovedNodes(vector< vector<string> > nodes, int layerIndex);

  /**
   * @brief 
   * 
   */
  void initRemovedNodeArrays(int arraySize);

  /**
   * @brief Get the Total Solution Count of a trained model
   * 
   * This is a very time consuming depth first search of the model's
   * structure
   * 
   * @param node Current node
   * @param matrixIndex Current transition matrix layer
   * @param count reference to total solution count
   * @author Porter Glines 2/26/20
   */
  void getTotalSolutionCountImpl(string node, int matrixIndex, int& count);
};

#endif