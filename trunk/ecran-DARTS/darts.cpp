#include "darts.h"

#include <QDebug>


/**
* @file darts.cpp
*
* @brief darts s'occupe du deroulement de la partie
*
* @author Bounoir Fabien
*
* @version 0.1
*
*/

/**
 * @brief constructeur de la classe Darts
 *
 * @fn Darts::Darts
 * @param parent
 */
Darts::Darts(QObject *parent) : QObject(parent), joueur(nullptr), nbJoueur(0), joueurActif(0), manche(1), pointLancer(0), voleeMax(0), nbVolees(0), ModeDeJeu("")
{
    solution = new Solution(this);
}

/**
 * @brief destructeur de la classe Darts
 *
 * @fn Darts::~Darts
 */
Darts::~Darts()
{

}

/**
 * @brief retourne la manche
 *
 * @fn Darts::getManche
 * @return int
 */
int Darts::getManche() const
{
    return manche;
}

/**
 * @brief retourne une liste des joueurs
 *
 * @fn Darts::getListJoueur
 * @return QList<Joueur>
 */
QList<Joueur> Darts::getListJoueur() const
{
    return joueurs;
}

/**
 * @brief retourne la volee Max
 *
 * @fn Darts::getVoleeMax
 * @return int
 */
int Darts::getVoleeMax()
{
    return voleeMax;
}

/**
 * @brief retourne le numero du joueur actif
 *
 * @fn Darts::getJoueurActif
 * @return int
 */
int Darts::getJoueurActif()
{
    return joueurActif;
}

/**
 * @brief Méthode qui retourne le nombre de volées de la partie
 *
 * @fn Darts::getNbVolees
 * @return int
 */
int Darts::getNbVolees()
{
    return nbVolees;
}

/**
 * @brief
 *
 * @fn Darts::getModeDeJeu
 * @return QString
 */
QString Darts::getModeDeJeu()
{
    return ModeDeJeu;
}

/**
 * @brief retourne l'objet solution
 *
 * @fn Darts::getSolution
 * @return solution
 */
Solution *Darts::getSolution() const
{
    return solution;
}

/**
 * @brief  Méthode qui permet de mettre à jour la volée Max
 *
 * @fn Darts::setVoleeMax
 * @param voleeMax
 */
void Darts::setVoleeMax(int voleeMax)
{
    this->voleeMax = voleeMax;
}

/**
 * @brief permet de mettre à jour le numero de manche
 *
 * @fn Darts::setManche
 * @param manche
 */
void Darts::setManche(int manche)
{
    this->manche = manche;
}


/**
 * @brief Méthode qui permet d'initialiser la partie
 *
 * @fn Darts::initialiserPartie
 * @param joueurList
 * @param modeJeu
 */
void Darts::initialiserPartie(QStringList joueurList, QString modeJeu)
{
    nbJoueur = joueurList.size() - 1;
    ModeDeJeu = modeJeu;
    qDebug() << "nombre de JOUEUR : " << nbJoueur;

    if(ModeDeJeu == "501" || ModeDeJeu == "301")
    {
        for(int i = 1; i < joueurList.size() ; i++)
        {
            Joueur player(joueurList.at(i), modeJeu.toInt(), 3);
            joueurs.push_back(player);
        }
        emit nouvellePartie();
    }
    else if(ModeDeJeu == "501_DOUBLE_OUT")
    {
        for(int i = 1; i < joueurList.size() ; i++)
        {
            Joueur player(joueurList.at(i), 501, 3);
            joueurs.push_back(player);
        }
        emit nouvellePartie();
    }
    else if(ModeDeJeu == "301_DOUBLE_OUT")
    {
        for(int i = 1; i < joueurList.size() ; i++)
        {
            Joueur player(joueurList.at(i), 301, 3);
            joueurs.push_back(player);
        }
        emit nouvellePartie();
    }
    else
    {
        qDebug() << "Erreur Mode De Jeu" << endl;
        reinitialiserPartie();
    }
    solution->trouverSolution(joueurs[joueurActif].getScore(),joueurs[joueurActif].getFlechette());
}

/**
 * @brief Méthode qui permet de remettre a zero les differents attribut et conteneur pour une nouvelle partie
 *
 * @fn Darts::reinitialiserPartie
 */
void Darts::reinitialiserPartie()
{
    joueurs.clear();
    joueur.clear();
    ModeDeJeu = "";
    nbJoueur = 0;
    joueurActif = 0;
    manche = 1;
    pointLancer = 0;
    nbVolees = 0;
    voleeMax = 0;
}

/**
 * @brief Méthode qui permet de traiter la reception d'impact
 *
 * @fn Darts::receptionnerImpact
 * @param cercle
 * @param point
 */
void Darts::receptionnerImpact(int cercle, int point)
{
    if(cercle == TRIPLE_POINT)
    {
        pointLancer = point * 3;
    }
    else if(cercle == DOUBLE_POINT)
    {
        pointLancer = point * 2;
    }
    else
    {
        pointLancer = point;
    }

    emit nouvelleImpact(cercle, point, pointLancer);
    qDebug() << joueurs[joueurActif].getNom() << " SCORE : "<<joueurs[joueurActif].getScore() - pointLancer << endl;
    joueurs[joueurActif].setScore(joueurs[joueurActif].getScore() - pointLancer);
    testerImpact(cercle);
    emit miseAJourPoint();
}

/**
 * @brief Methode qui teste si le joueur a gagné
 *
 * @fn Darts::testerImpact
 * @param cercle
 * @param pointLancer
 */
void Darts::testerImpact(int cercle)
{
    if(joueurs[joueurActif].getScore()  == 0 && (cercle == DOUBLE_POINT || cercle == 2) && (ModeDeJeu == "501_DOUBLE_OUT" || ModeDeJeu == "301_DOUBLE_OUT")) //fin avec double
    {
        gererVoleeMax();
        nbVolees++;
        emit finPartie(joueurs[joueurActif].getNom(), getVoleeMax());
        emit etatPartieFini();
    }
    else if(joueurs[joueurActif].getScore()  == 0 && (ModeDeJeu == "501" || ModeDeJeu == "301"))    //fin sans double
    {
        gererVoleeMax();
        nbVolees++;
        emit finPartie(joueurs[joueurActif].getNom(), getVoleeMax());
        emit etatPartieFini();
    }
    else
    {
        enleverPointImpact();
        gererManche();
    }
}

/**
 * @brief Méthode qui met à jour le score du joueur
 *
 * @fn Darts::enleverPointImpact
 */
void Darts::enleverPointImpact()
{
    if(joueurs[joueurActif].getScore() <= 0) //score Volées inferieur au score = Volée annulée
    {
        joueurs[joueurActif].setScore(joueurs[joueurActif].getScoreManchePrecedente());
        emit voleeAnnulee();

        joueurs[joueurActif].setNbFlechette(0);
    }
    else
    {
        nbVolees++;
        joueurs[joueurActif].setNbFlechette(joueurs[joueurActif].getFlechette() - 1);
        solution->trouverSolution(joueurs[joueurActif].getScore(),joueurs[joueurActif].getFlechette());
    }
}

/**
 * @brief Méthode qui permet de gerer le changement de manche en fonction des flechettes de chaque joueur
 *
 * @fn Darts::gererManche
 */
void Darts::gererManche()
{
    if(joueurs[joueurActif].getFlechette() == 0) //fin de la volées du joueur
    {
        joueurs[joueurActif].setNbFlechette(3);

        gererVoleeMax();

        joueurs[joueurActif].addHistoriqueVolees((joueurs[joueurActif].getScoreManchePrecedente() - joueurs[joueurActif].getScore()));

        joueurs[joueurActif].setScoreManchePrecedente(joueurs[joueurActif].getScore());

        if(joueurActif == nbJoueur - 1)  //equivalent a la fin de manche
        {
            joueurActif = 0;
            setManche(getManche() + 1);
            emit changerJoueurActif();
            calculerMoyenneVolees();
            emit nouvelleManche();
            solution->trouverSolution(joueurs[joueurActif].getScore(),joueurs[joueurActif].getFlechette());
        }
        else
        {
            joueurActif++;
            emit changerJoueurActif();
            solution->trouverSolution(joueurs[joueurActif].getScore(),joueurs[joueurActif].getFlechette());
        }
    }
}

/**
 * @brief Méthode qui calcule la moyenne des Volées de chaque joueur
 *
 * @fn Darts::calculerMoyenneVolees
 */
void Darts::calculerMoyenneVolees()
{
    for(int i = 0; i <= joueurs.size() - 1; i++)
    {
        float moyenneVolee = 0;

        for(int j = 0; j < joueurs[i].getHistoriqueVolees().size(); j++)
        {
            moyenneVolee += joueurs[i].getHistoriqueVolees()[j];
        }

        moyenneVolee = moyenneVolee / joueurs[i].getHistoriqueVolees().size();
        joueurs[i].setMoyenneVolee(moyenneVolee);
    }
    emit miseAJourMoyenneVolee();
}

/**
 * @brief Méthode qui teste la Volée pour savoir si elle est superieur à la Volée Max
 *
 * @fn Darts::gererVoleeMax
 */
void Darts::gererVoleeMax()
{
    if((joueurs[joueurActif].getScoreManchePrecedente() - joueurs[joueurActif].getScore()) > getVoleeMax())
    {
        setVoleeMax(joueurs[joueurActif].getScoreManchePrecedente() - joueurs[joueurActif].getScore());
    }
}

/**
 * @brief Méthode qui arrête la partie
 *
 * @fn Darts::arreterPartie
 */
void Darts::arreterPartie()
{
    emit finPartie(calculerGagnant(), getVoleeMax());
}

/**
 * @brief Méthode qui calcule le gagnant de la partie
 *
 * @fn Darts::calculerGagnant
 * @return QString
 */
QString Darts::calculerGagnant()
{
    QString gagnant;
    int scoreGagnant = 1000;
    for(int i = 0; i <= joueurs.size() - 1; i++)
    {
        if(scoreGagnant > joueurs[i].getScore())
        {
            scoreGagnant = joueurs[i].getScore();
            gagnant = joueurs[i].getNom();
        }
    }
    return gagnant;
}
