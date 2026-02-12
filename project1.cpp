#include <iostream>
#include <string>
using namespace std;

class Post
{
public:
    string posterLastName; // last name of the person who posted
    int messageId;         // unique per poster
    string date;           // format: YYYYMMDD
    string time;           // format: HHMM
    string message;        // full post text (may contain hashtags)
    bool isDeleted;        // true if post is deleted
    // Constructors
    Post() : messageId(0), isDeleted(false) {}
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
    Person() : numFollows(0), numOutgoing(0), numIncoming(0) {}
    // Utility prototypes
    bool followsPerson(const string &lastName) const;
};

class Postings
{
public:
    Post posts[500];
    int numPosts;
    // Constructors
    Postings() : numPosts(0) {}
    // Post management
    int addPost(const Post &p);
    int findPost(const string &posterLastName, int messageId) const;
    void deletePost(int postIndex);
};

class SocialMediaSystem
{
public:
    Person persons[100];
    int numPersons;
    Postings postings;
    // Constructors
    SocialMediaSystem() : numPersons(0) {}
    // Person management
    int findPerson(const string &lastName) const;
    // Command handlers
    void addPost(const string &posterLastName,
                 int messageId,
                 const string &date,
                 const string &time,
                 const string &message);
    void deletePost(const string &posterLastName,
                    int messageId);
    void printPerson(const string &lastName) const;
    void searchHashtag(const string &hashtag) const;
    void statistics(int k) const;
    // Helpers
    bool isEarlier(const Post &a, const Post &b) const;
};

// Person method implementations
bool Person::followsPerson(const string &lastName) const
{
    for (int i = 0; i < numFollows; i++)
    {
        if (follows[i] == lastName)
            return true;
    }
    return false;
}

// Postings method implementations
int Postings::addPost(const Post &p)
{
    posts[numPosts] = p;
    return numPosts++;
}

int Postings::findPost(const string &posterLastName, int messageId) const
{
    for (int i = 0; i < numPosts; i++)
    {
        if (posts[i].posterLastName == posterLastName && posts[i].messageId == messageId)
            return i;
    }
    return -1;
}

void Postings::deletePost(int postIndex)
{
    if (postIndex >= 0 && postIndex < numPosts)
        posts[postIndex].isDeleted = true;
}

// SocialMediaSystem method implementations
int SocialMediaSystem::findPerson(const string &lastName) const
{
    for (int i = 0; i < numPersons; i++)
    {
        if (persons[i].lastName == lastName)
            return i;
    }
    return -1;
}

bool SocialMediaSystem::isEarlier(const Post &a, const Post &b) const
{
    if (a.date != b.date)
        return a.date < b.date;
    return a.time < b.time;
}

void SocialMediaSystem::addPost(const string &posterLastName,
                                int messageId,
                                const string &date,
                                const string &time,
                                const string &message)
{
    // Create post
    Post p;
    p.posterLastName = posterLastName;
    p.messageId = messageId;
    p.date = date;
    p.time = time;
    p.message = message;
    p.isDeleted = false;

    // Add to postings
    int postIdx = postings.addPost(p);

    // Add to poster's outgoing
    int posterIdx = findPerson(posterLastName);
    if (posterIdx != -1)
    {
        persons[posterIdx].outgoingPostIdx[persons[posterIdx].numOutgoing++] = postIdx;
    }

    // Add to followers' incoming
    for (int i = 0; i < numPersons; i++)
    {
        if (persons[i].followsPerson(posterLastName))
        {
            persons[i].incomingPostIdx[persons[i].numIncoming++] = postIdx;
        }
    }
}

void SocialMediaSystem::deletePost(const string &posterLastName,
                                   int messageId)
{
    int postIdx = postings.findPost(posterLastName, messageId);
    if (postIdx != -1)
    {
        postings.deletePost(postIdx);
    }
}

void SocialMediaSystem::printPerson(const string &lastName) const
{
    int personIdx = findPerson(lastName);
    if (personIdx == -1)
        return;

    cout << "PERSON " << lastName << endl;

    // Collect and sort outgoing posts
    int outgoingIndices[500];
    int outgoingCount = 0;
    for (int i = 0; i < persons[personIdx].numOutgoing; i++)
    {
        int postIdx = persons[personIdx].outgoingPostIdx[i];
        if (!postings.posts[postIdx].isDeleted)
        {
            outgoingIndices[outgoingCount++] = postIdx;
        }
    }

    // Sort outgoing by date/time
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

    // Collect and sort incoming posts
    int incomingIndices[500];
    int incomingCount = 0;
    for (int i = 0; i < persons[personIdx].numIncoming; i++)
    {
        int postIdx = persons[personIdx].incomingPostIdx[i];
        if (!postings.posts[postIdx].isDeleted)
        {
            incomingIndices[incomingCount++] = postIdx;
        }
    }

    // Sort incoming by date/time
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

void SocialMediaSystem::searchHashtag(const string &hashtag) const
{
    // Find matching posts
    int matchingIndices[500];
    int matchCount = 0;

    for (int i = 0; i < postings.numPosts; i++)
    {
        if (postings.posts[i].isDeleted)
            continue;

        // Check if hashtag appears as full word in message
        const string &msg = postings.posts[i].message;
        size_t pos = 0;
        bool found = false;

        while ((pos = msg.find(hashtag, pos)) != string::npos)
        {
            // Check if it's a full word (start of string or preceded by space)
            // and (end of string or followed by space/punctuation)
            bool validStart = (pos == 0) || (msg[pos - 1] == ' ');
            bool validEnd = (pos + hashtag.length() == msg.length()) ||
                            (msg[pos + hashtag.length()] == ' ') ||
                            (msg[pos + hashtag.length()] == '.') ||
                            (msg[pos + hashtag.length()] == ',') ||
                            (msg[pos + hashtag.length()] == '!') ||
                            (msg[pos + hashtag.length()] == '?');

            if (validStart && validEnd)
            {
                found = true;
                break;
            }
            pos++;
        }

        if (found)
        {
            matchingIndices[matchCount++] = i;
        }
    }

    // Sort by date/time
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

void SocialMediaSystem::statistics(int k) const
{
    // Calculate active post counts
    struct PersonStat
    {
        string lastName;
        int count;
    };

    // Top K Active Posters
    PersonStat activePosters[100];
    for (int i = 0; i < numPersons; i++)
    {
        activePosters[i].lastName = persons[i].lastName;
        activePosters[i].count = 0;
        for (int j = 0; j < persons[i].numOutgoing; j++)
        {
            int postIdx = persons[i].outgoingPostIdx[j];
            if (!postings.posts[postIdx].isDeleted)
                activePosters[i].count++;
        }
    }

    // Sort active posters
    for (int i = 0; i < numPersons - 1; i++)
    {
        for (int j = i + 1; j < numPersons; j++)
        {
            if (activePosters[i].count < activePosters[j].count ||
                (activePosters[i].count == activePosters[j].count && activePosters[i].lastName > activePosters[j].lastName))
            {
                PersonStat temp = activePosters[i];
                activePosters[i] = activePosters[j];
                activePosters[j] = temp;
            }
        }
    }

    cout << "TOP " << k << " ACTIVE POSTERS" << endl;
    int limit = (k < numPersons) ? k : numPersons;
    for (int i = 0; i < limit; i++)
    {
        cout << (i + 1) << ") " << activePosters[i].lastName << " " << activePosters[i].count << endl;
    }

    // Top K Receivers
    PersonStat receivers[100];
    for (int i = 0; i < numPersons; i++)
    {
        receivers[i].lastName = persons[i].lastName;
        receivers[i].count = 0;
        for (int j = 0; j < persons[i].numIncoming; j++)
        {
            int postIdx = persons[i].incomingPostIdx[j];
            if (!postings.posts[postIdx].isDeleted)
                receivers[i].count++;
        }
    }

    // Sort receivers
    for (int i = 0; i < numPersons - 1; i++)
    {
        for (int j = i + 1; j < numPersons; j++)
        {
            if (receivers[i].count < receivers[j].count ||
                (receivers[i].count == receivers[j].count && receivers[i].lastName > receivers[j].lastName))
            {
                PersonStat temp = receivers[i];
                receivers[i] = receivers[j];
                receivers[j] = temp;
            }
        }
    }

    cout << "TOP " << k << " RECEIVERS" << endl;
    for (int i = 0; i < limit; i++)
    {
        cout << (i + 1) << ") " << receivers[i].lastName << " " << receivers[i].count << endl;
    }

    // Top K Most Followed
    PersonStat followed[100];
    for (int i = 0; i < numPersons; i++)
    {
        followed[i].lastName = persons[i].lastName;
        followed[i].count = 0;
        for (int j = 0; j < numPersons; j++)
        {
            if (i != j && persons[j].followsPerson(persons[i].lastName))
                followed[i].count++;
        }
    }

    // Sort followed
    for (int i = 0; i < numPersons - 1; i++)
    {
        for (int j = i + 1; j < numPersons; j++)
        {
            if (followed[i].count < followed[j].count ||
                (followed[i].count == followed[j].count && followed[i].lastName > followed[j].lastName))
            {
                PersonStat temp = followed[i];
                followed[i] = followed[j];
                followed[j] = temp;
            }
        }
    }

    cout << "TOP " << k << " MOST FOLLOWED" << endl;
    for (int i = 0; i < limit; i++)
    {
        cout << (i + 1) << ") " << followed[i].lastName << " " << followed[i].count << endl;
    }
}

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