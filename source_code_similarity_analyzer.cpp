#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <set>

using namespace std;


// ============================================================
// STRUCTURE TO STORE ANALYSIS RESULT
// ============================================================

struct AnalysisResult
{
    int lines1 = 0;
    int lines2 = 0;
    int matchingLines = 0;

    int commonKeywords = 0;
    int totalKeywords = 0;

    double lineSimilarity = 0.0;
    double keywordSimilarity = 0.0;
    double overallSimilarity = 0.0;
};


// ============================================================
// CONVERT TEXT TO LOWERCASE
// ============================================================

string toLowerCase(string text)
{
    transform(
        text.begin(),
        text.end(),
        text.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(tolower(c));
        }
    );

    return text;
}


// ============================================================
// NORMALIZE A LINE
// Removes comments and extra spaces
// ============================================================

string normalizeLine(string line)
{
    // Remove // comments
    size_t commentPos = line.find("//");

    if (commentPos != string::npos)
    {
        line = line.substr(0, commentPos);
    }

    string result;

    bool previousSpace = false;

    for (unsigned char c : line)
    {
        if (isspace(c))
        {
            if (!previousSpace)
            {
                result += ' ';
                previousSpace = true;
            }
        }
        else
        {
            result += static_cast<char>(tolower(c));
            previousSpace = false;
        }
    }

    // Remove leading and trailing spaces
    size_t start = result.find_first_not_of(' ');
    size_t end = result.find_last_not_of(' ');

    if (start == string::npos)
    {
        return "";
    }

    return result.substr(start, end - start + 1);
}


// ============================================================
// READ SOURCE FILE
// ============================================================

vector<string> readSourceFile(const string& filename)
{
    ifstream file(filename);

    vector<string> lines;

    if (!file)
    {
        return lines;
    }

    string line;

    while (getline(file, line))
    {
        string cleanedLine = normalizeLine(line);

        if (!cleanedLine.empty())
        {
            lines.push_back(cleanedLine);
        }
    }

    file.close();

    return lines;
}


// ============================================================
// EXTRACT C/C++ KEYWORDS
// ============================================================

set<string> extractKeywords(const vector<string>& lines)
{
    const set<string> keywords =
    {
        "auto",
        "bool",
        "break",
        "case",
        "char",
        "class",
        "const",
        "continue",
        "default",
        "do",
        "double",
        "else",
        "enum",
        "float",
        "for",
        "if",
        "int",
        "long",
        "namespace",
        "private",
        "protected",
        "public",
        "return",
        "short",
        "signed",
        "sizeof",
        "static",
        "struct",
        "switch",
        "template",
        "this",
        "throw",
        "try",
        "typedef",
        "unsigned",
        "using",
        "virtual",
        "void",
        "volatile",
        "while",
        "include",
        "string",
        "vector",
        "cout",
        "cin"
    };

    set<string> found;

    for (const string& line : lines)
    {
        string word;

        for (size_t i = 0; i <= line.size(); i++)
        {
            char c;

            if (i < line.size())
            {
                c = line[i];
            }
            else
            {
                c = ' ';
            }

            if (isalnum(static_cast<unsigned char>(c)) || c == '_')
            {
                word += static_cast<char>(
                    tolower(static_cast<unsigned char>(c))
                );
            }
            else
            {
                if (!word.empty())
                {
                    if (keywords.count(word))
                    {
                        found.insert(word);
                    }

                    word.clear();
                }
            }
        }
    }

    return found;
}


// ============================================================
// COUNT MATCHING LINES
// ============================================================

int countMatchingLines(
    const vector<string>& file1,
    const vector<string>& file2)
{
    vector<bool> used(file2.size(), false);

    int matches = 0;

    for (const string& line1 : file1)
    {
        for (size_t j = 0; j < file2.size(); j++)
        {
            if (!used[j] && line1 == file2[j])
            {
                used[j] = true;

                matches++;

                break;
            }
        }
    }

    return matches;
}


// ============================================================
// ANALYZE TWO FILES
// ============================================================

AnalysisResult analyzeFiles(
    const vector<string>& file1,
    const vector<string>& file2)
{
    AnalysisResult result;

    result.lines1 = file1.size();
    result.lines2 = file2.size();

    // --------------------------------------------------------
    // LINE COMPARISON
    // --------------------------------------------------------

    result.matchingLines =
        countMatchingLines(file1, file2);

    int maximumLines =
        max(result.lines1, result.lines2);

    if (maximumLines > 0)
    {
        result.lineSimilarity =
            (static_cast<double>(result.matchingLines)
             / maximumLines) * 100;
    }


    // --------------------------------------------------------
    // KEYWORD COMPARISON
    // --------------------------------------------------------

    set<string> keywords1 =
        extractKeywords(file1);

    set<string> keywords2 =
        extractKeywords(file2);

    int common = 0;

    for (const string& word : keywords1)
    {
        if (keywords2.count(word))
        {
            common++;
        }
    }

    int unionSize =
        keywords1.size()
        + keywords2.size()
        - common;

    result.commonKeywords = common;

    result.totalKeywords = unionSize;


    // --------------------------------------------------------
    // KEYWORD SIMILARITY
    // --------------------------------------------------------

    if (unionSize > 0)
    {
        result.keywordSimilarity =
            (static_cast<double>(common)
             / unionSize) * 100;
    }


    // --------------------------------------------------------
    // OVERALL SIMILARITY
    // --------------------------------------------------------

    result.overallSimilarity =
        (0.70 * result.lineSimilarity)
        +
        (0.30 * result.keywordSimilarity);


    return result;
}


// ============================================================
// SAVE REPORT
// ============================================================

void saveReport(
    const vector<string>& filenames,
    const vector<vector<AnalysisResult>>& results)
{
    ofstream report("comparison_report.txt");

    if (!report)
    {
        cout << "\nUnable to create report file.\n";

        return;
    }

    report << "============================================================\n";

    report << "       SOURCE CODE SIMILARITY COMPARISON REPORT\n";

    report << "============================================================\n\n";


    report << "Files Compared:\n";

    for (size_t i = 0; i < filenames.size(); i++)
    {
        report << i + 1
               << ". "
               << filenames[i]
               << endl;
    }


    report << "\n============================================================\n";

    report << "                    COMPARISON RESULTS\n";

    report << "============================================================\n\n";


    // Compare every pair
    for (size_t i = 0; i < filenames.size(); i++)
    {
        for (size_t j = i + 1; j < filenames.size(); j++)
        {
            const AnalysisResult& result =
                results[i][j];

            report << filenames[i]
                   << " VS "
                   << filenames[j]
                   << endl;

            report << "--------------------------------------------\n";

            report << fixed
                   << setprecision(2);

            report << "Line Similarity     : "
                   << result.lineSimilarity
                   << "%\n";

            report << "Keyword Similarity  : "
                   << result.keywordSimilarity
                   << "%\n";

            report << "Overall Similarity  : "
                   << result.overallSimilarity
                   << "%\n\n";
        }
    }


    report << "============================================================\n";

    report << "Note: This is an educational source-code comparison tool.\n";

    report << "The similarity score does not prove plagiarism or authorship.\n";

    report << "============================================================\n";

    report.close();

    cout << "\nReport saved as comparison_report.txt\n";
}


// ============================================================
// MAIN FUNCTION
// ============================================================

int main()
{
    int choice;

    cout << "============================================================\n";

    cout << "     SOURCE CODE SIMILARITY AND COMPARISON ANALYZER\n";

    cout << "============================================================\n";


    do
    {
        cout << "\n1. Compare Multiple Source Files";

        cout << "\n2. Exit";

        cout << "\n\nEnter your choice: ";

        cin >> choice;


        // ====================================================
        // OPTION 1
        // ====================================================

        if (choice == 1)
        {
            int numberOfFiles;

            cout << "\nEnter number of files to compare: ";

            cin >> numberOfFiles;


            // Check minimum number of files
            if (numberOfFiles < 2)
            {
                cout << "\nYou must enter at least 2 files.\n";

                continue;
            }


            // =================================================
            // STORE FILE NAMES
            // =================================================

            vector<string> filenames(numberOfFiles);


            for (int i = 0; i < numberOfFiles; i++)
            {
                cout << "Enter file "
                     << i + 1
                     << ": ";

                cin >> filenames[i];
            }


            // =================================================
            // READ ALL FILES
            // =================================================

            vector<vector<string>> sourceFiles(numberOfFiles);

            bool error = false;


            for (int i = 0; i < numberOfFiles; i++)
            {
                sourceFiles[i] =
                    readSourceFile(filenames[i]);


                if (sourceFiles[i].empty())
                {
                    cout << "\nCould not read file: "
                         << filenames[i]
                         << endl;

                    error = true;
                }
            }


            if (error)
            {
                cout << "\nPlease check your file names and try again.\n";

                continue;
            }


            // =================================================
            // CREATE RESULT TABLE
            // =================================================

            vector<vector<AnalysisResult>> results(
                numberOfFiles,
                vector<AnalysisResult>(numberOfFiles)
            );


            // =================================================
            // COMPARE EVERY FILE WITH EVERY OTHER FILE
            // =================================================

            cout << "\n============================================================\n";

            cout << "                 COMPARISON RESULTS\n";

            cout << "============================================================\n";


            for (int i = 0; i < numberOfFiles; i++)
            {
                for (int j = i + 1; j < numberOfFiles; j++)
                {
                    results[i][j] =
                        analyzeFiles(
                            sourceFiles[i],
                            sourceFiles[j]
                        );


                    cout << "\n";

                    cout << filenames[i]
                         << " VS "
                         << filenames[j]
                         << endl;

                    cout << "--------------------------------------------\n";


                    cout << fixed
                         << setprecision(2);


                    cout << "Line Similarity    : "
                         << results[i][j].lineSimilarity
                         << "%\n";


                    cout << "Keyword Similarity : "
                         << results[i][j].keywordSimilarity
                         << "%\n";


                    cout << "Overall Similarity : "
                         << results[i][j].overallSimilarity
                         << "%\n";
                }
            }


            // =================================================
            // SAVE REPORT
            // =================================================

            char saveChoice;

            cout << "\nDo you want to save the report? (y/n): ";

            cin >> saveChoice;


            if (saveChoice == 'y' ||
                saveChoice == 'Y')
            {
                saveReport(
                    filenames,
                    results
                );
            }
        }


        // ====================================================
        // OPTION 2
        // ====================================================

        else if (choice == 2)
        {
            cout << "\nThank you for using the analyzer.\n";
        }


        // ====================================================
        // INVALID OPTION
        // ====================================================

        else
        {
            cout << "\nInvalid choice. Please try again.\n";
        }

    }
    while (choice != 2);


    return 0;
}