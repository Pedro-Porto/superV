#include <cairomm/context.h>
#include <gtkmm.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

class data{  // classe abstrata
    public:

    virtual void save(const string& filename){

    }

    virtual void load(const string& filename){
        
    }

    virtual ~data() = default;
};

class manipulates_history: public data{
    private: 

    vector <string> history;


    public:

    void add(const string &copy){
        history.push_back(copy);
    }

    void remove(){
        if(history.size() > 0){
            history.pop_back();
        } 
    }

    string display(){
        string copy;
        for(long unsigned int i = 0; i < history.size(); i++){  
            copy = copy + history[i] + "\n";
        }
        return copy;
    }

    string get_last_copy(){
        if(history.size() > 0){
            return history.back(); 
        } else {
            return "";
        }
    }
    
    static string xor_encrypt_decrypt(const string &text, char key){  
        string output = text;
        for(long unsigned int i = 0; i < text.size(); i++){
            output[i] = text[i] ^ key; // faz o XOR
        }
        return output;
    }

    void save(const string& filename) override{
        ofstream file(filename);                    // escrita no arquivo
        char key = 'K';

        if(!file){
            cout << "Erro ao abrir o arquivo para escrita." << endl;
            return;
        }

        for(long unsigned int i = 0; i < history.size(); i++){  
            string encrypted = xor_encrypt_decrypt(history[i], key);
            file << encrypted << endl;
        }

        file.close();
    } 
    
    void load(const string& filename) override{
        ifstream file(filename);                // leitura do arquivo
        history.clear(); 
        string line;
        char key = 'K'; 

        if(!file){
            cout << "Erro ao abrir o arquivo para leitura." << endl;
            return;
        }

        while(getline(file, line)){
            history.push_back(xor_encrypt_decrypt(line,key)); 
        }

        file.close();
    }
};

class MainApp : public Gtk::Window {
   public:
    MainApp() : clipboard_content("") {
        set_app_paintable(true);
        auto screen1 = get_screen();
        auto visual = screen1->get_rgba_visual();
        if (visual) {
            gtk_widget_set_visual(Gtk::Widget::gobj(), visual->gobj());
        } else {
            g_warning("RGBA not found");
        }

        auto css_provider = Gtk::CssProvider::create();
        css_provider->load_from_data(R"(
            .list-row {
                background-color: lightgray;
                border-radius: 10px;
                color: black;
                padding: 10px;
            }
            .box-container {
                border-radius: 20px;
                margin: 5px;
                padding: 5px;
            }
            .app-container {
                background-color: #2a2a2a;
                border-radius: 20px;
                padding: 10px;
            }
        )");
        auto screen = Gdk::Display::get_default()->get_default_screen();
        Gtk::StyleContext::add_provider_for_screen(
            screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

        set_default_size(300, 400);
        set_decorated(false);
        set_position(Gtk::WIN_POS_CENTER);

        app_container.set_orientation(Gtk::ORIENTATION_VERTICAL);
        app_container.get_style_context()->add_class("app-container");

        input_field.set_placeholder_text("Digite um numero ou texto");
        input_field.set_max_length(10);
        input_field.set_hexpand(true);

        add_button.set_label("Adicionar");
        add_button.signal_clicked().connect(
            sigc::mem_fun(*this, &MainApp::on_add_button_clicked));

        auto input_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
        input_box->pack_start(input_field, true, true);
        input_box->pack_start(add_button, false, false);
        app_container.pack_start(*input_box, Gtk::PACK_SHRINK);

        scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC,
                                   Gtk::POLICY_AUTOMATIC);
        scrolled_window.add(box);

        box.set_orientation(Gtk::ORIENTATION_VERTICAL);
        box.set_spacing(10);

        box.get_style_context()->add_class("box-container");

        for (int i = 0; i < 20; ++i) {
            auto event_box =
                Gtk::make_managed<Gtk::EventBox>();

            event_box->set_size_request(200, 100);
            event_box->get_style_context()->add_class("list-row");

            auto label = Gtk::make_managed<Gtk::Label>(std::to_string(i + 1));
            event_box->add(*label);

            event_box->signal_button_press_event().connect(
                sigc::bind<Gtk::Label*>(sigc::mem_fun(*this, &MainApp::on_copy_selected), label));

            box.pack_start(*event_box, Gtk::PACK_SHRINK);

            history.add(to_string(i + 1));
        }
        app_container.pack_start(scrolled_window, Gtk::PACK_EXPAND_WIDGET);
        add(app_container);

        signal_key_press_event().connect(
            sigc::mem_fun(*this, &MainApp::on_key_press), false);
        
        show_all_children();
    }

    ~MainApp(){
        history.save("history.txt");  // 
    }

    string clipboard_content;

    void add_copy(const string &nome, bool n){
        clipboard_content = nome;

        if(n){
            cout << "selecionado para colar: " << clipboard_content << endl;
        }
        
        auto event_box =
            Gtk::make_managed<Gtk::EventBox>();

        event_box->set_size_request(200, 100);
        event_box->get_style_context()->add_class("list-row");

        auto label = Gtk::make_managed<Gtk::Label>(clipboard_content);
        event_box->add(*label);

        event_box->signal_button_press_event().connect(
            sigc::bind<Gtk::Label*>(sigc::mem_fun(*this, &MainApp::on_copy_selected), label));

        box.pack_start(*event_box, Gtk::PACK_SHRINK);
        show_all_children();

        history.add(clipboard_content);          
    }

    void remove_copy(){
        history.remove();
    }

    void save_copies(){
        history.save("history.txt");
    }

    void load_copies(){
        history.load("history.txt");
        history.display();
    }

   protected:
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Box box;
    Gtk::Box app_container;

    Gtk::Entry input_field;
    Gtk::Button add_button;

    manipulates_history history;
    string last_copied;

    bool is_copied = false;  // ver se ja deu ctrl c ou nao, naquilo que esta querendo colar
    int flag = 0;
    int flag2 = 0;

    bool on_copy_selected(GdkEventButton* event, Gtk::Label *label){
        clipboard_content = label->get_text();
        cout << "selecionado: " << clipboard_content << endl;
        flag = 0;

        return false;
    }

    bool on_key_press(GdkEventKey *key_event){
        if((key_event->state & GDK_CONTROL_MASK) && key_event->keyval == GDK_KEY_c){
            is_copied = true;
            if(is_copied && !clipboard_content.empty()){
                cout << "copiado: " << clipboard_content << endl;
                flag = 1;
                flag2 = 1;
            } else {
                cout << "Nada para copiar." << endl;
            }

            return true;
        } else if((key_event->state & GDK_CONTROL_MASK) && key_event->keyval == GDK_KEY_v){
            if(flag == 1 && !clipboard_content.empty()){
                last_copied = paste_copy();
            } else if(flag2 == 1 && !clipboard_content.empty()){
                add_copy(last_copied, false);
                cout << "colado: " << last_copied << endl;
            } else {
                cout << "Nada para colar." << endl;
            }

            return true;
        }

        return false;
    }

    string paste_copy(){
        if(!clipboard_content.empty()){
            auto event_box =
                Gtk::make_managed<Gtk::EventBox>();

            event_box->set_size_request(200, 100);
            event_box->get_style_context()->add_class("list-row");

            auto label = Gtk::make_managed<Gtk::Label>(clipboard_content);
            event_box->add(*label);

            event_box->signal_button_press_event().connect(
                sigc::bind<Gtk::Label*>(sigc::mem_fun(*this, &MainApp::on_copy_selected), label));

            box.pack_start(*event_box, Gtk::PACK_SHRINK);
            show_all_children();

            history.add(clipboard_content);

            cout << "colado: " << clipboard_content << endl;           
        }

        return clipboard_content;
    }

    void on_add_button_clicked(){
        string text = input_field.get_text();

        if (!text.empty()) {
            add_copy(text, true);
            input_field.set_text(""); 
        } else {
            cout << "Nada para adicionar pela interface." << endl;
        }
    }
};
    
int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv); 
    MainApp janela;

    janela.add_copy("teste", true);

    // janela.save_copies();

    /// janela.load_copies();

    return app->run(janela);
}
