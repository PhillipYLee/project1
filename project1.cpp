#include <iostream>
#include <string>
using namespace std;

class Post
{
public:
    string posterLastName; // last name of the person who posted
    int messageId; // unique per poster
    string date; // format: YYYYMMDD
    string time; // format: HHMM
    string message; // full post text (may contain hashtags)
    bool isDeleted; // true if post is deleted

    // Constructors
    Post()
    {
        messageId = 0;
        isDeleted = false;
    }
};

class Person
{
public:
    string lastName;

    // Follow relationships
    string follows[10]; // last names this person follows
    int numFollows;

    // Post references (indices into Postings.posts)
    int outgoingPostIdx[500];
    int numOutgoing;
    int incomingPostIdx[500];
    int numIncoming;

    // Constructors
    Person()
    {
        numFollows = 0;
        numOutgoing = 0;
        numIncoming = 0;
    }

    // Utility prototypes
    bool followsPerson(const string &lastName) const
    {
        for (int i = 0; i < numFollows; i++)
        {
            if (follows[i] == lastName)
                return true;
        }
        return false;
    }
};

class Postings
{
public:
    Post posts[500];
    int numPosts;

    // Constructors
    Postings()
    {
        numPosts = 0;
    }

    // Post management
    int addPost(const Post &p)
    {
        posts[numPosts] = p;
        numPosts++;
        return numPosts - 1;
    }
    int findPost(const string &posterLastName, int messageId) const
    {
        for (int i = 0; i < numPosts; i++)
        {
            if (posts[i].posterLastName == posterLastName && posts[i].messageId == messageId)
                return i;
        }
        return -1;
    }
    void deletePost(int postIndex)
    {
        if (postIndex >= 0 && postIndex < numPosts) 
        {
            posts[postIndex].isDeleted = true;
        }
    }
};

class SocialMediaSystem
{
public:
    Person persons[100];
    int numPersons;
    Postings postings;

    // Constructors
    SocialMediaSystem()
    {
        numPersons = 0;
    }
    
    int findPerson(const string &lastName) const
    {
        for (int i = 0; i < numPersons; i++)
        {
            if (persons[i].lastName == lastName)
                return i;
        }
        return -1;
    }


    // Command handlers

    // Format
    // A PosterLastName messageId YYYYMMDD HHMM message text...
    // Behavior
    // • Create a new post with the given information (mark isDeleted = false).
    // • Store it in the global posts array.
    // • Add the post’s index to the poster’s outgoing list.
    // • For every person who follows PosterLastName, add the post’s index to their incoming list.
    void addPost(const string &posterLastName, int messageId, const string &date, const string &time, const string &message)
    {
        Post p;
        p.posterLastName = posterLastName;
        p.messageId = messageId;
        p.date = date;
        p.time = time;
        p.message = message;
        p.isDeleted = false;

        int postIdx = postings.addPost(p);

        int posterIdx = findPerson(posterLastName);
        if (posterIdx != -1)
        {
            persons[posterIdx].outgoingPostIdx[persons[posterIdx].numOutgoing] = postIdx;
            persons[posterIdx].numOutgoing++;
        }

        for (int i = 0; i < numPersons; i++)
        {
            if (persons[i].followsPerson(posterLastName))
            {
                persons[i].incomingPostIdx[persons[i].numIncoming] = postIdx;
                persons[i].numIncoming++;
            }
        }
    }

    // Format
    // D PosterLastName messageId
    // Behavior
    // • Find the post whose poster and message ID match.
    // • Mark the post as deleted (isDeleted = true).
    // • Do not remove it from anyone’s incoming/outgoing arrays
    void deletePost(const string &posterLastName, int messageId)
    {
        int postIdx = postings.findPost(posterLastName, messageId);
        if (postIdx != -1)
        {
            postings.deletePost(postIdx);
        }
    }

    // Format
    // P LastName
    // Behavior
    // Print:
    // 1. PERSON LastName
    // 2. OUTGOING:
    // o All non-deleted outgoing posts
    // o Sorted by date/time (earliest first)
    // 3. INCOMING:
    // o All non-deleted incoming posts
    // o Sorted by date/time (earliest first)
    // Each post must be printed as:
    // PosterLastName (messageId) [YYYYMMDD HHMM]: message text
    void printPerson(const string &lastName) const
    {
        int personIdx = findPerson(lastName);
        if (personIdx == -1)
            return;

        cout << "PERSON " << lastName << endl;

        int outgoingIndices[500];
        int outgoingCount = 0;
        for (int i = 0; i < persons[personIdx].numOutgoing; i++)
        {
            int postIdx = persons[personIdx].outgoingPostIdx[i];
            if (!postings.posts[postIdx].isDeleted)
            {
                outgoingIndices[outgoingCount] = postIdx;
                outgoingCount++;
            }
        }

        for (int i = 0; i < outgoingCount - 1; i++)
        {
            for (int j = i + 1; j < outgoingCount; j++)
            {
                if (!isEarlier(postings.posts[outgoingIndices[i]], postings.posts[outgoingIndices[j]]))
                {
                    int temp = outgoingIndices[i];
                    outgoingIndices[i] = outgoingIndices[j];
                    outgoingIndices[j] = temp;
                }
            }
        }

        cout << "OUTGOING:" << endl;
        for (int i = 0; i < outgoingCount; i++)
        {
            const Post &p = postings.posts[outgoingIndices[i]];
            cout << p.posterLastName << " (" << p.messageId << ") [" << p.date << " " << p.time << "]: " << p.message << endl;
        }

        int incomingIndices[500];
        int incomingCount = 0;
        for (int i = 0; i < persons[personIdx].numIncoming; i++)
        {
            int postIdx = persons[personIdx].incomingPostIdx[i];
            if (!postings.posts[postIdx].isDeleted)
            {
                incomingIndices[incomingCount] = postIdx;
                incomingCount++;
            }
        }

        for (int i = 0; i < incomingCount - 1; i++)
        {
            for (int j = i + 1; j < incomingCount; j++)
            {
                if (!isEarlier(postings.posts[incomingIndices[i]], postings.posts[incomingIndices[j]]))
                {
                    int temp = incomingIndices[i];
                    incomingIndices[i] = incomingIndices[j];
                    incomingIndices[j] = temp;
                }
            }
        }

        cout << "INCOMING:" << endl;
        for (int i = 0; i < incomingCount; i++)
        {
            const Post &p = postings.posts[incomingIndices[i]];
            cout << p.posterLastName << " (" << p.messageId << ") [" << p.date << " " << p.time << "]: " << p.message << endl;
        }
    }

    // Format
    // S hashtag
    // Example: S #AI
    // Behavior
    // • Search all non-deleted posts for the hashtag as a full word in the message.
    // • Collect matching posts.
    // • Sort matches by date/time (earliest first).
    // • Print:
    // POSTS WITH hashtag
    // PosterLastName (messageId) [YYYYMMDD HHMM]: message text
    void searchHashtag(const string &hashtag) const
    {
        int matchingIndices[500];
        int matchCount = 0;

        for (int i = 0; i < postings.numPosts; i++)
        {
            if (postings.posts[i].isDeleted)
                continue;
            const string &msg = postings.posts[i].message;
            bool found = false;
            size_t searchPos = 0;
            while (searchPos < msg.length())
            {
                size_t pos = msg.find(hashtag, searchPos);
                if (pos == string::npos)
                    break;
                bool validStart = (pos == 0) || (msg[pos - 1] == ' ');
                size_t endPos = pos + hashtag.length();
                bool validEnd = (endPos >= msg.length()) ||
                                (msg[endPos] == ' ') ||
                                (msg[endPos] == '.') ||
                                (msg[endPos] == ',') ||
                                (msg[endPos] == '!') ||
                                (msg[endPos] == '?');

                if (validStart && validEnd)
                {
                    found = true;
                    break;
                }
                searchPos = pos + 1;
            }

            if (found)
            {
                matchingIndices[matchCount] = i;
                matchCount++;
            }
        }

        for (int i = 0; i < matchCount - 1; i++)
        {
            for (int j = i + 1; j < matchCount; j++)
            {
                if (!isEarlier(postings.posts[matchingIndices[i]], postings.posts[matchingIndices[j]]))
                {
                    int temp = matchingIndices[i];
                    matchingIndices[i] = matchingIndices[j];
                    matchingIndices[j] = temp;
                }
            }
        }

        cout << "POSTS WITH " << hashtag << endl;
        for (int i = 0; i < matchCount; i++)
        {
            const Post &p = postings.posts[matchingIndices[i]];
            cout << p.posterLastName << " (" << p.messageId << ") [" << p.date << " " << p.time << "]: " << p.message << endl;
        }
    }
    
    // Format
    // X K
    // • X is the command letter.
    // • K is a positive integer (for example, 5 for “top 5”).
    // • If K is larger than the number of persons, print stats for all persons.
    // What X Must Print
    // For every X K command, the program prints three ranked lists, in this order:
    // 1. Top K active posters
    // 2. Top K receivers
    // 3. Top K most followed persons
    // Each list is printed separately, with a header and numbered entries.
    // Ranks go from 1 up to min(K, numPersons).
    void statistics(int k) const
    {
        string names[100];
        int activeCounts[100];
        int receiveCounts[100];
        int followCounts[100];
        
        for (int i = 0; i < numPersons; i++)
        {
            names[i] = persons[i].lastName;
            activeCounts[i] = 0;
            for (int j = 0; j < persons[i].numOutgoing; j++)
            {
                int postIdx = persons[i].outgoingPostIdx[j];
                if (!postings.posts[postIdx].isDeleted)
                    activeCounts[i]++;
            }
        }

        for (int i = 0; i < numPersons - 1; i++)
        {
            for (int j = i + 1; j < numPersons; j++)
            {
                if (activeCounts[i] < activeCounts[j] ||
                    (activeCounts[i] == activeCounts[j] && names[i] > names[j]))
                {
                    int tempCount = activeCounts[i];
                    activeCounts[i] = activeCounts[j];
                    activeCounts[j] = tempCount;
                    
                    string tempName = names[i];
                    names[i] = names[j];
                    names[j] = tempName;
                }
            }
        }

        cout << "TOP " << k << " ACTIVE POSTERS" << endl;
        int limit = k;
        if (limit > numPersons)
            limit = numPersons;
        for (int i = 0; i < limit; i++)
        {
            cout << (i + 1) << ") " << names[i] << " " << activeCounts[i] << endl;
        }

        for (int i = 0; i < numPersons; i++)
        {
            names[i] = persons[i].lastName;
            receiveCounts[i] = 0;
            for (int j = 0; j < persons[i].numIncoming; j++)
            {
                int postIdx = persons[i].incomingPostIdx[j];
                if (!postings.posts[postIdx].isDeleted)
                    receiveCounts[i]++;
            }
        }

        for (int i = 0; i < numPersons - 1; i++)
        {
            for (int j = i + 1; j < numPersons; j++)
            {
                if (receiveCounts[i] < receiveCounts[j] ||
                    (receiveCounts[i] == receiveCounts[j] && names[i] > names[j]))
                {
                    int tempCount = receiveCounts[i];
                    receiveCounts[i] = receiveCounts[j];
                    receiveCounts[j] = tempCount;
                    
                    string tempName = names[i];
                    names[i] = names[j];
                    names[j] = tempName;
                }
            }
        }

        cout << "TOP " << k << " RECEIVERS" << endl;
        for (int i = 0; i < limit; i++)
        {
            cout << (i + 1) << ") " << names[i] << " " << receiveCounts[i] << endl;
        }

        for (int i = 0; i < numPersons; i++)
        {
            names[i] = persons[i].lastName;
            followCounts[i] = 0;
            for (int j = 0; j < numPersons; j++)
            {
                if (i != j && persons[j].followsPerson(persons[i].lastName))
                    followCounts[i]++;
            }
        }

        for (int i = 0; i < numPersons - 1; i++)
        {
            for (int j = i + 1; j < numPersons; j++)
            {
                if (followCounts[i] < followCounts[j] ||
                    (followCounts[i] == followCounts[j] && names[i] > names[j]))
                {
                    int tempCount = followCounts[i];
                    followCounts[i] = followCounts[j];
                    followCounts[j] = tempCount;
                    
                    string tempName = names[i];
                    names[i] = names[j];
                    names[j] = tempName;
                }
            }
        }

        cout << "TOP " << k << " MOST FOLLOWED" << endl;
        for (int i = 0; i < limit; i++)
        {
            cout << (i + 1) << ") " << names[i] << " " << followCounts[i] << endl;
        }
    }

    bool isEarlier(const Post &a, const Post &b) const
    {
        if (a.date != b.date)
            return a.date < b.date;
        return a.time < b.time;
    }
};

int main()
{
SocialMediaSystem sys;
int N;
cin >> N;
sys.numPersons = N;
for (int i = 0; i < N; i++)
{
cin >> sys.persons[i].lastName;
int k;
cin >> k;
for (int j = 0; j < k; j++)
cin >> sys.persons[i].follows[sys.persons[i].numFollows++];
}
int C;
cin >> C;
while (C--)
{
char cmd;
cin >> cmd;
if (cmd == 'A')
{
string ln, d, t, msg;
int id;
cin >> ln >> id >> d >> t;
cin.ignore();
getline(cin, msg);
sys.addPost(ln, id, d, t, msg);
}
else if (cmd == 'D')
{
string ln;
int id;
cin >> ln >> id;
sys.deletePost(ln, id);
}
else if (cmd == 'P')
{
string ln;
cin >> ln;
sys.printPerson(ln);
}
else if (cmd == 'S')
{
string tag;
cin >> tag;
sys.searchHashtag(tag);
}
else if (cmd == 'X')
{
int k;
cin >> k;
sys.statistics(k);
}
}
return 0;
}