#include <iostream>
#include <fstream>
#include "httplib.h"


using namespace httplib;
using namespace std;

vector<string> notes;

void saveNotes() {
    ofstream f("notes.txt");
    for (string& s : notes)
        f << s << endl;
    f.close();
}

void getNotes() {
    notes.clear();
    ifstream f("notes.txt");
    string tmp;
    while (!f.eof()) {
        getline(f, tmp);
        if (tmp.length())
            notes.push_back(tmp);
    }
    f.close();
}

void indexHandler(const Request& req, Response& res) {
    getNotes();
    string notesHtml;
    for (int i = 0; i < notes.size(); i++) {
        notesHtml += "<div class=\"note\"><p>" + notes[i] +
                     "</p><a href=\"/delete_note/" +
                     to_string(i) + "\">Удалить</a></div>";
    }
    Result resp = Client("localhost").Get("/page.html");
    string html = resp->body;
    html.replace(html.find("notes"), 5, notesHtml);
    res.set_content(html, "text/html");
}

void addNoteHandler(const httplib::Request& req, httplib::Response& res) {
    notes.push_back(req.get_param_value("note"));
    saveNotes();
    res.set_redirect("/");
}

void deleteNoteHandler(const httplib::Request& req, httplib::Response& res) {
    int ind = stoi(req.matches[1]);
    if (ind < notes.size())
        notes.erase(notes.begin() + ind);
    saveNotes();
    res.set_redirect("/");
}

int main() {
    Server svr;
    svr.Get("/", indexHandler);
    svr.Post("/add_note", addNoteHandler);
    svr.Get(R"(/delete_note/(\d+))", deleteNoteHandler);
    svr.set_base_dir("./static");

    cout << "Server started on http://localhost" << endl;
    svr.listen("localhost", 80);

    return 0;
}
